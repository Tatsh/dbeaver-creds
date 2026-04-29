#include "compat.h"

#include <corecrypto/ccaes.h>
#include <corecrypto/ccmode.h>

#include "backend.h"

int dbc_decrypt_aes_128_cbc(const unsigned char *key,
                            const unsigned char *iv,
                            const unsigned char *cipher,
                            size_t cipher_len,
                            unsigned char *plain,
                            size_t *plain_len) {
    if (cipher_len == 0 || cipher_len % 16 != 0) {
        return -1;
    }
    const struct ccmode_cbc *cbc = ccaes_cbc_decrypt_mode();
    cccbc_ctx_decl(cbc->size, ctx);
    cccbc_iv_decl(cbc->block_size, iv_ctx);
    cccbc_init(cbc, ctx, 16, key);
    cccbc_set_iv(cbc, iv_ctx, iv);
    cccbc_update(cbc, ctx, iv_ctx, cipher_len / 16, cipher, plain);
    cccbc_ctx_clear(cbc->size, ctx);
    cccbc_iv_clear(cbc->block_size, iv_ctx);

    /* corecrypto operates on whole blocks only, so PKCS#7 padding is removed manually. */
    unsigned char pad = plain[cipher_len - 1];
    if (pad < 1 || pad > 16) {
        return -1;
    }
    for (size_t i = 0; i < pad; ++i) {
        if (plain[cipher_len - 1 - i] != pad) {
            return -1;
        }
    }
    *plain_len = cipher_len - pad;
    return 0;
}
