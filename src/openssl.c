#include "compat.h"

#include <openssl/evp.h>

#include "backend.h"

int dbc_decrypt_aes_128_cbc(const unsigned char *key,
                            const unsigned char *iv,
                            const unsigned char *cipher,
                            size_t cipher_len,
                            unsigned char *plain,
                            size_t *plain_len) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }
    int rc = -1;
    int len = 0;
    int total = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv) != 1) {
        goto done;
    }
    if (EVP_DecryptUpdate(ctx, plain, &len, cipher, (int)cipher_len) != 1) {
        goto done;
    }
    total = len;
    if (EVP_DecryptFinal_ex(ctx, plain + total, &len) != 1) {
        goto done;
    }
    total += len;
    *plain_len = (size_t)total;
    rc = 0;
done:
    EVP_CIPHER_CTX_free(ctx);
    return rc;
}
