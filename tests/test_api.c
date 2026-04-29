#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define DBC_ENV_KEY "APPDATA"
#define DBC_REL_DIR "DBeaverData\\workspace6\\General\\.dbeaver"
#define DBC_REL_FILE DBC_REL_DIR "\\credentials-config.json"
#define DBC_PATHSEP '\\'
#define DBC_SETENV(k, v) SetEnvironmentVariableA((k), (v))
#define DBC_UNSETENV(k) SetEnvironmentVariableA((k), NULL)
static int dbc_mkdir(const char *p) {
    return CreateDirectoryA(p, NULL) ? 0 : (GetLastError() == ERROR_ALREADY_EXISTS ? 0 : -1);
}
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#if defined(__APPLE__)
#define DBC_ENV_KEY "HOME"
#define DBC_REL_DIR "Library/DBeaverData/workspace6/General/.dbeaver"
#else
#define DBC_ENV_KEY "XDG_DATA_HOME"
#define DBC_REL_DIR "DBeaverData/workspace6/General/.dbeaver"
#endif
#define DBC_REL_FILE DBC_REL_DIR "/credentials-config.json"
#define DBC_PATHSEP '/'
#define DBC_SETENV(k, v) setenv((k), (v), 1)
#define DBC_UNSETENV(k) unsetenv(k)
static int dbc_mkdir(const char *p) {
    return mkdir(p, 0755) == 0 || errno == EEXIST ? 0 : -1;
}
#endif

#include <cmocka.h>

#include <dbeaver-creds.h>

/* clang-format off */
static const unsigned char k_cipher[48] = {
    0xbd, 0x70, 0x91, 0x60, 0xb7, 0x2c, 0x65, 0x06,
    0x22, 0x1c, 0x87, 0x7a, 0x17, 0xc2, 0x13, 0x9b,
    0x20, 0xaa, 0x02, 0x39, 0xa4, 0xbf, 0xd0, 0xcf,
    0xd4, 0x63, 0xc7, 0x23, 0x26, 0xf0, 0xfc, 0xdd,
    0x3a, 0xd4, 0xd4, 0x12, 0xe3, 0x4d, 0x1a, 0x01,
    0x88, 0x9e, 0x75, 0xa3, 0x68, 0x02, 0x3a, 0x64
};
/* clang-format on */

typedef struct {
    char root[1024];
    char saved_env[1024];
    int env_was_set;
} fixture_t;

static int make_dirs(const char *path) {
    char buf[1024];
    size_t n = strlen(path);
    if (n + 1 > sizeof(buf)) {
        return -1;
    }
    memcpy(buf, path, n + 1);
    for (size_t i = 1; i <= n; ++i) {
        if (buf[i] == '/' || buf[i] == '\\' || buf[i] == '\0') {
            char saved = buf[i];
            buf[i] = '\0';
            if (dbc_mkdir(buf) != 0) {
                return -1;
            }
            buf[i] = saved;
        }
    }
    return 0;
}

static int write_file(const char *path, const void *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        return -1;
    }
    int ok = fwrite(data, 1, len, f) == len;
    fclose(f);
    return ok ? 0 : -1;
}

static int unique_root(char *out, size_t cap) {
#if defined(_WIN32)
    static unsigned long counter = 0;
    char tmp[MAX_PATH];
    DWORD n = GetTempPathA(sizeof(tmp), tmp);
    if (n == 0 || n >= sizeof(tmp)) {
        return -1;
    }
    snprintf(out, cap, "%sdbc_test_%lu_%lu", tmp, (unsigned long)GetCurrentProcessId(), ++counter);
    return CreateDirectoryA(out, NULL) ? 0 : -1;
#else
    const char *tmp = getenv("TMPDIR");
    if (!tmp || !*tmp) {
        tmp = "/tmp";
    }
    snprintf(out, cap, "%s/dbc_test_XXXXXX", tmp);
    return mkdtemp(out) ? 0 : -1;
#endif
}

static int setup(void **state) {
    fixture_t *fx = calloc(1, sizeof(*fx));
    if (!fx || unique_root(fx->root, sizeof(fx->root)) != 0) {
        free(fx);
        return -1;
    }
    const char *cur = getenv(DBC_ENV_KEY);
    if (cur) {
        strncpy(fx->saved_env, cur, sizeof(fx->saved_env) - 1);
        fx->env_was_set = 1;
    }
    DBC_SETENV(DBC_ENV_KEY, fx->root);
    *state = fx;
    return 0;
}

static int teardown(void **state) {
    fixture_t *fx = *state;
    if (fx->env_was_set) {
        DBC_SETENV(DBC_ENV_KEY, fx->saved_env);
    } else {
        DBC_UNSETENV(DBC_ENV_KEY);
    }
    free(fx);
    return 0;
}

static int write_creds_blob(const fixture_t *fx, const void *data, size_t len) {
    char dir[1280];
    char file[1408];
    snprintf(dir, sizeof(dir), "%s%c%s", fx->root, DBC_PATHSEP, DBC_REL_DIR);
    snprintf(file, sizeof(file), "%s%c%s", fx->root, DBC_PATHSEP, DBC_REL_FILE);
    if (make_dirs(dir) != 0) {
        return -1;
    }
    return write_file(file, data, len);
}

static void test_returns_decrypted_json(void **state) {
    fixture_t *fx = *state;
    assert_int_equal(write_creds_blob(fx, k_cipher, sizeof(k_cipher)), 0);
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_DECRYPTION_FAILED;
    char *json = get_dbeaver_credentials(nullptr, &err);
    assert_non_null(json);
    assert_int_equal(err, DBEAVER_CREDENTIALS_OK);
    assert_string_equal(json, "{\"hello\":\"world\"}");
    free(json);
}

static void test_explicit_path_is_used(void **state) {
    fixture_t *fx = *state;
    /* Write the encrypted blob under a non-default name; default discovery would miss it. */
    char file[1280];
    snprintf(file, sizeof(file), "%s%c%s", fx->root, DBC_PATHSEP, "explicit.bin");
    assert_int_equal(write_file(file, k_cipher, sizeof(k_cipher)), 0);
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_DECRYPTION_FAILED;
    char *json = get_dbeaver_credentials(file, &err);
    assert_non_null(json);
    assert_int_equal(err, DBEAVER_CREDENTIALS_OK);
    assert_string_equal(json, "{\"hello\":\"world\"}");
    free(json);
}

static void test_returns_null_when_file_missing(void **state) {
    (void)state;
    /* No file written under the temp root: discovery succeeds but read fails. */
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
    char *json = get_dbeaver_credentials(nullptr, &err);
    assert_null(json);
    assert_int_equal(err, DBEAVER_CREDENTIALS_FILE_READ_FAILED);
}

static void test_returns_null_on_unaligned_ciphertext(void **state) {
    fixture_t *fx = *state;
    const unsigned char garbage[5] = {1, 2, 3, 4, 5};
    assert_int_equal(write_creds_blob(fx, garbage, sizeof(garbage)), 0);
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
    char *json = get_dbeaver_credentials(nullptr, &err);
    assert_null(json);
    assert_int_equal(err, DBEAVER_CREDENTIALS_INVALID_CIPHERTEXT);
}

static void test_returns_null_on_undecryptable_ciphertext(void **state) {
    fixture_t *fx = *state;
    unsigned char tampered[sizeof(k_cipher)];
    memcpy(tampered, k_cipher, sizeof(tampered));
    tampered[sizeof(tampered) - 1] ^= 0xff;
    assert_int_equal(write_creds_blob(fx, tampered, sizeof(tampered)), 0);
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
    char *json = get_dbeaver_credentials(nullptr, &err);
    assert_null(json);
    assert_int_equal(err, DBEAVER_CREDENTIALS_DECRYPTION_FAILED);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_returns_decrypted_json, setup, teardown),
        cmocka_unit_test_setup_teardown(test_explicit_path_is_used, setup, teardown),
        cmocka_unit_test_setup_teardown(test_returns_null_when_file_missing, setup, teardown),
        cmocka_unit_test_setup_teardown(test_returns_null_on_unaligned_ciphertext, setup, teardown),
        cmocka_unit_test_setup_teardown(
            test_returns_null_on_undecryptable_ciphertext, setup, teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
