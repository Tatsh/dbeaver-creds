/* Tests for the BCrypt failure paths in src/win32.c. Each `goto done`
 * branch is exercised by mocking the relevant BCrypt call to return a
 * non-success NTSTATUS via cmocka's --wrap mechanism. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <bcrypt.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <cmocka.h>

#include "backend.h"

NTSTATUS __wrap_BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE *phAlgorithm,
                                            LPCWSTR pszAlgId,
                                            LPCWSTR pszImplementation,
                                            ULONG dwFlags) {
    (void)phAlgorithm;
    (void)pszAlgId;
    (void)pszImplementation;
    (void)dwFlags;
    return mock_type(NTSTATUS);
}

NTSTATUS __wrap_BCryptSetProperty(BCRYPT_HANDLE hObject,
                                  LPCWSTR pszProperty,
                                  PUCHAR pbInput,
                                  ULONG cbInput,
                                  ULONG dwFlags) {
    (void)hObject;
    (void)pszProperty;
    (void)pbInput;
    (void)cbInput;
    (void)dwFlags;
    return mock_type(NTSTATUS);
}

NTSTATUS __wrap_BCryptGenerateSymmetricKey(BCRYPT_ALG_HANDLE hAlgorithm,
                                           BCRYPT_KEY_HANDLE *phKey,
                                           PUCHAR pbKeyObject,
                                           ULONG cbKeyObject,
                                           PUCHAR pbSecret,
                                           ULONG cbSecret,
                                           ULONG dwFlags) {
    (void)hAlgorithm;
    (void)phKey;
    (void)pbKeyObject;
    (void)cbKeyObject;
    (void)pbSecret;
    (void)cbSecret;
    (void)dwFlags;
    return mock_type(NTSTATUS);
}

NTSTATUS __wrap_BCryptDecrypt(BCRYPT_KEY_HANDLE hKey,
                              PUCHAR pbInput,
                              ULONG cbInput,
                              VOID *pPaddingInfo,
                              PUCHAR pbIV,
                              ULONG cbIV,
                              PUCHAR pbOutput,
                              ULONG cbOutput,
                              ULONG *pcbResult,
                              ULONG dwFlags) {
    (void)hKey;
    (void)pbInput;
    (void)cbInput;
    (void)pPaddingInfo;
    (void)pbIV;
    (void)cbIV;
    (void)pbOutput;
    (void)cbOutput;
    (void)pcbResult;
    (void)dwFlags;
    return mock_type(NTSTATUS);
}

static const unsigned char k_key[16] = {0};
static const unsigned char k_iv[16] = {0};
static const unsigned char k_cipher[16] = {0};

static void invoke_and_assert_failure(void) {
    unsigned char plain[16];
    size_t plain_len = sizeof(plain);
    int rc = dbc_decrypt_aes_128_cbc(k_key, k_iv, k_cipher, sizeof(k_cipher), plain, &plain_len);
    assert_int_equal(rc, -1);
}

static void test_open_algorithm_provider_failure(void **state) {
    (void)state;
    will_return(__wrap_BCryptOpenAlgorithmProvider, (NTSTATUS)0xC0000001);
    invoke_and_assert_failure();
}

static void test_set_property_failure(void **state) {
    (void)state;
    will_return(__wrap_BCryptOpenAlgorithmProvider, (NTSTATUS)0);
    will_return(__wrap_BCryptSetProperty, (NTSTATUS)0xC0000001);
    invoke_and_assert_failure();
}

static void test_generate_symmetric_key_failure(void **state) {
    (void)state;
    will_return(__wrap_BCryptOpenAlgorithmProvider, (NTSTATUS)0);
    will_return(__wrap_BCryptSetProperty, (NTSTATUS)0);
    will_return(__wrap_BCryptGenerateSymmetricKey, (NTSTATUS)0xC0000001);
    invoke_and_assert_failure();
}

static void test_decrypt_failure(void **state) {
    (void)state;
    will_return(__wrap_BCryptOpenAlgorithmProvider, (NTSTATUS)0);
    will_return(__wrap_BCryptSetProperty, (NTSTATUS)0);
    will_return(__wrap_BCryptGenerateSymmetricKey, (NTSTATUS)0);
    will_return(__wrap_BCryptDecrypt, (NTSTATUS)0xC0000001);
    invoke_and_assert_failure();
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_open_algorithm_provider_failure),
        cmocka_unit_test(test_set_property_failure),
        cmocka_unit_test(test_generate_symmetric_key_failure),
        cmocka_unit_test(test_decrypt_failure),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
