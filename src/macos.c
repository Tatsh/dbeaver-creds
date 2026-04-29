#include "compat.h"

#include <CommonCrypto/CommonCryptor.h>

#include "backend.h"

int dbc_decrypt_aes_128_cbc(const unsigned char *key,
                            const unsigned char *iv,
                            const unsigned char *cipher,
                            size_t cipher_len,
                            unsigned char *plain,
                            size_t *plain_len) {
    if (cipher_len == 0 || cipher_len % kCCBlockSizeAES128 != 0) {
        return -1;
    }
    /* CommonCrypto's kCCOptionPKCS7Padding strips padding but does not
       validate it strictly, so tampered ciphertexts whose last decrypted
       byte happens to fall in [1, 16] are silently truncated. Decrypt
       block-aligned and verify PKCS#7 padding by hand to match the
       OpenSSL/BCrypt behaviour. */
    size_t out_size = 0;
    CCCryptorStatus status = CCCrypt(kCCDecrypt,
                                     kCCAlgorithmAES,
                                     0,
                                     key,
                                     kCCKeySizeAES128,
                                     iv,
                                     cipher,
                                     cipher_len,
                                     plain,
                                     cipher_len,
                                     &out_size);
    if (status != kCCSuccess || out_size != cipher_len) {
        return -1;  // LCOV_EXCL_LINE
    }
    unsigned char pad = plain[cipher_len - 1];
    if (pad < 1 || pad > kCCBlockSizeAES128) {
        return -1;
    }
    for (size_t i = 0; i < pad; ++i) {
        if (plain[cipher_len - 1 - i] != pad) {
            return -1;  // LCOV_EXCL_LINE
        }
    }
    *plain_len = cipher_len - pad;
    return 0;
}
