#include "compat.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <bcrypt.h>
#include <string.h>

#include "backend.h"

#ifndef NT_SUCCESS
#define NT_SUCCESS(s) (((NTSTATUS)(s)) >= 0)
#endif

int dbc_decrypt_aes_128_cbc(const unsigned char *key,
                            const unsigned char *iv,
                            const unsigned char *cipher,
                            size_t cipher_len,
                            unsigned char *plain,
                            size_t *plain_len) {
    if (cipher_len == 0 || cipher_len % 16 != 0) {
        return -1;
    }
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_KEY_HANDLE key_h = nullptr;
    int rc = -1;
    NTSTATUS s = BCryptOpenAlgorithmProvider(&alg, BCRYPT_AES_ALGORITHM, nullptr, 0);
    if (!NT_SUCCESS(s)) {
        goto done;
    }
    s = BCryptSetProperty(
        alg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
    if (!NT_SUCCESS(s)) {
        goto done;
    }
    s = BCryptGenerateSymmetricKey(alg, &key_h, nullptr, 0, (PUCHAR)key, 16, 0);
    if (!NT_SUCCESS(s)) {
        goto done;
    }
    UCHAR iv_buf[16];
    memcpy(iv_buf, iv, 16);
    ULONG out_len = 0;
    s = BCryptDecrypt(key_h,
                      (PUCHAR)cipher,
                      (ULONG)cipher_len,
                      nullptr,
                      iv_buf,
                      sizeof(iv_buf),
                      plain,
                      (ULONG)cipher_len,
                      &out_len,
                      BCRYPT_BLOCK_PADDING);
    if (!NT_SUCCESS(s)) {
        goto done;
    }
    *plain_len = out_len;
    rc = 0;
done:
    if (key_h) {
        BCryptDestroyKey(key_h);
    }
    if (alg) {
        BCryptCloseAlgorithmProvider(alg, 0);
    }
    return rc;
}
