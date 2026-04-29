#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <cmocka.h>

#include "backend.h"

/* clang-format off */
static const unsigned char k_cipher[48] = {
    0xbd, 0x70, 0x91, 0x60, 0xb7, 0x2c, 0x65, 0x06,
    0x22, 0x1c, 0x87, 0x7a, 0x17, 0xc2, 0x13, 0x9b,
    0x20, 0xaa, 0x02, 0x39, 0xa4, 0xbf, 0xd0, 0xcf,
    0xd4, 0x63, 0xc7, 0x23, 0x26, 0xf0, 0xfc, 0xdd,
    0x3a, 0xd4, 0xd4, 0x12, 0xe3, 0x4d, 0x1a, 0x01,
    0x88, 0x9e, 0x75, 0xa3, 0x68, 0x02, 0x3a, 0x64
};
static const unsigned char k_key[16] = {
    0xba, 0xbb, 0x4a, 0x9f, 0x77, 0x4a, 0xb8, 0x53,
    0xc9, 0x6c, 0x2d, 0x65, 0x3d, 0xfe, 0x54, 0x4a
};
/* clang-format on */
static const unsigned char k_iv[16] = {0};

static void test_decrypts_known_ciphertext(void **state) {
    (void)state;
    unsigned char plain[48];
    size_t plain_len = sizeof(plain);
    int rc = dbc_decrypt_aes_128_cbc(k_key, k_iv, k_cipher, sizeof(k_cipher), plain, &plain_len);
    assert_int_equal(rc, 0);
    /* Plaintext is 16 'A' bytes followed by `{"hello":"world"}` (17 bytes). */
    assert_int_equal((int)plain_len, 16 + 17);
    for (int i = 0; i < 16; ++i) {
        assert_int_equal(plain[i], 'A');
    }
    assert_memory_equal(plain + 16, "{\"hello\":\"world\"}", 17);
}

static void test_rejects_zero_length(void **state) {
    (void)state;
    unsigned char plain[16] = {0};
    size_t plain_len = sizeof(plain);
    int rc = dbc_decrypt_aes_128_cbc(k_key, k_iv, k_cipher, 0, plain, &plain_len);
    assert_int_not_equal(rc, 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_decrypts_known_ciphertext),
        cmocka_unit_test(test_rejects_zero_length),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
