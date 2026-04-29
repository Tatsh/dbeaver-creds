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
    size_t out_size = 0;
    CCCryptorStatus status = CCCrypt(kCCDecrypt,
                                     kCCAlgorithmAES,
                                     kCCOptionPKCS7Padding,
                                     key,
                                     kCCKeySizeAES128,
                                     iv,
                                     cipher,
                                     cipher_len,
                                     plain,
                                     cipher_len,
                                     &out_size);
    if (status != kCCSuccess) {
        return -1;
    }
    *plain_len = out_size;
    return 0;
}
