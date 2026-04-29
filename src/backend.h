#ifndef DBC_BACKEND_H
#define DBC_BACKEND_H

#include <stddef.h>

int dbc_decrypt_aes_128_cbc(const unsigned char *key,
                            const unsigned char *iv,
                            const unsigned char *cipher,
                            size_t cipher_len,
                            unsigned char *plain,
                            size_t *plain_len);

#endif
