#ifndef DBC_TEST_WRAPPERS_H
#define DBC_TEST_WRAPPERS_H

#include <stddef.h>

int __wrap_dbc_decrypt_aes_128_cbc(const unsigned char *key,
                                   const unsigned char *iv,
                                   const unsigned char *cipher,
                                   size_t cipher_len,
                                   unsigned char *plain,
                                   size_t *plain_len);

#endif
