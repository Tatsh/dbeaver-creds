#include "wrappers.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <cmocka.h>

int __wrap_dbc_decrypt_aes_128_cbc(const unsigned char *key,
                                   const unsigned char *iv,
                                   const unsigned char *cipher,
                                   size_t cipher_len,
                                   unsigned char *plain,
                                   size_t *plain_len) {
    (void)key;
    (void)iv;
    (void)cipher;
    (void)cipher_len;
    int rc = mock_type(int);
    if (rc == 0) {
        const unsigned char *bytes = mock_ptr_type(const unsigned char *);
        size_t len = mock_type(size_t);
        memcpy(plain, bytes, len);
        *plain_len = len;
    }
    return rc;
}
