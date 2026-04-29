/* Tests for the OpenSSL EVP failure paths in src/openssl.c. Each
 * "context allocation/init/update/finalisation failed" branch is
 * exercised by mocking the relevant EVP_* call to return a non-success
 * value via cmocka's --wrap mechanism. */
#include "compat.h"

#include <openssl/evp.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "backend.h"

EVP_CIPHER_CTX *__wrap_EVP_CIPHER_CTX_new(void) {
    return mock_ptr_type(EVP_CIPHER_CTX *);
}

void __wrap_EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx) {
    /* Mock only; the test passes a sentinel pointer that must not reach the
     * real free routine. */
    (void)ctx;
}

int __wrap_EVP_DecryptInit_ex(EVP_CIPHER_CTX *ctx,
                              const EVP_CIPHER *type,
                              ENGINE *impl,
                              const unsigned char *key,
                              const unsigned char *iv) {
    (void)ctx;
    (void)type;
    (void)impl;
    (void)key;
    (void)iv;
    return mock_type(int);
}

int __wrap_EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx,
                             unsigned char *out,
                             int *outl,
                             const unsigned char *in,
                             int inl) {
    (void)ctx;
    (void)out;
    (void)outl;
    (void)in;
    (void)inl;
    return mock_type(int);
}

int __wrap_EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl) {
    (void)ctx;
    (void)outm;
    (void)outl;
    return mock_type(int);
}

static const unsigned char k_key[16] = {0};
static const unsigned char k_iv[16] = {0};
static const unsigned char k_cipher[16] = {0};

static EVP_CIPHER_CTX *fake_ctx(void) {
    /* Sentinel value: non-null so dbc_decrypt_aes_128_cbc proceeds past the
     * NULL-check, but never dereferenced because every EVP call is mocked. */
    return (EVP_CIPHER_CTX *)(uintptr_t)0x1;
}

static void invoke_and_assert_failure(void) {
    unsigned char plain[16];
    size_t plain_len = sizeof(plain);
    int rc = dbc_decrypt_aes_128_cbc(k_key, k_iv, k_cipher, sizeof(k_cipher), plain, &plain_len);
    assert_int_equal(rc, -1);
}

/* Covers src/openssl.c line 14 - cipher_len validation. */
static void test_rejects_zero_length(void **state) {
    (void)state;
    unsigned char plain[16];
    size_t plain_len = sizeof(plain);
    int rc = dbc_decrypt_aes_128_cbc(k_key, k_iv, k_cipher, 0, plain, &plain_len);
    assert_int_equal(rc, -1);
}

/* Covers src/openssl.c lines 33-35 - success path (`rc = 0`, `*plain_len`
 * assignment) when every EVP call succeeds. */
static void test_success_path(void **state) {
    (void)state;
    will_return(__wrap_EVP_CIPHER_CTX_new, fake_ctx());
    will_return(__wrap_EVP_DecryptInit_ex, 1);
    will_return(__wrap_EVP_DecryptUpdate, 1);
    will_return(__wrap_EVP_DecryptFinal_ex, 1);
    unsigned char plain[16];
    size_t plain_len = sizeof(plain);
    int rc = dbc_decrypt_aes_128_cbc(k_key, k_iv, k_cipher, sizeof(k_cipher), plain, &plain_len);
    assert_int_equal(rc, 0);
}

/* Covers src/openssl.c line 18 - EVP_CIPHER_CTX_new() returns NULL. */
static void test_ctx_new_returns_null(void **state) {
    (void)state;
    will_return(__wrap_EVP_CIPHER_CTX_new, NULL);
    invoke_and_assert_failure();
}

/* Covers src/openssl.c line 24 - EVP_DecryptInit_ex returns 0. */
static void test_decrypt_init_failure(void **state) {
    (void)state;
    will_return(__wrap_EVP_CIPHER_CTX_new, fake_ctx());
    will_return(__wrap_EVP_DecryptInit_ex, 0);
    invoke_and_assert_failure();
}

/* Covers src/openssl.c line 27 - EVP_DecryptUpdate returns 0. */
static void test_decrypt_update_failure(void **state) {
    (void)state;
    will_return(__wrap_EVP_CIPHER_CTX_new, fake_ctx());
    will_return(__wrap_EVP_DecryptInit_ex, 1);
    will_return(__wrap_EVP_DecryptUpdate, 0);
    invoke_and_assert_failure();
}

/* Covers src/openssl.c EVP_DecryptFinal_ex failure goto. */
static void test_decrypt_final_failure(void **state) {
    (void)state;
    will_return(__wrap_EVP_CIPHER_CTX_new, fake_ctx());
    will_return(__wrap_EVP_DecryptInit_ex, 1);
    will_return(__wrap_EVP_DecryptUpdate, 1);
    will_return(__wrap_EVP_DecryptFinal_ex, 0);
    invoke_and_assert_failure();
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_rejects_zero_length),
        cmocka_unit_test(test_success_path),
        cmocka_unit_test(test_ctx_new_returns_null),
        cmocka_unit_test(test_decrypt_init_failure),
        cmocka_unit_test(test_decrypt_update_failure),
        cmocka_unit_test(test_decrypt_final_failure),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
