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

#include "wrappers.h"

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
    snprintf(out,
             cap,
             "%sdbc_mock_%lu_%lu",
             tmp,
             (unsigned long)GetCurrentProcessId(),
             ++counter);
    return CreateDirectoryA(out, NULL) ? 0 : -1;
#else
    const char *tmp = getenv("TMPDIR");
    if (!tmp || !*tmp) {
        tmp = "/tmp";
    }
    snprintf(out, cap, "%s/dbc_mock_XXXXXX", tmp);
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

/* The mocked backend yields a controlled plaintext, so the ciphertext bytes
 * here only need to satisfy api.c's length check (multiple of 16, non-zero). */
static void test_strips_16_byte_prefix(void **state) {
    fixture_t *fx = *state;
    unsigned char dummy[32] = {0};
    assert_int_equal(write_creds_blob(fx, dummy, sizeof(dummy)), 0);

    static const unsigned char fake_plain[] = "PREFIX_SIXTEEN_X{\"k\":42}";
    will_return(__wrap_dbc_decrypt_aes_128_cbc, 0);
    will_return(__wrap_dbc_decrypt_aes_128_cbc, fake_plain);
    will_return(__wrap_dbc_decrypt_aes_128_cbc, (size_t)(sizeof(fake_plain) - 1));

    char *json = get_dbeaver_credentials(nullptr);
    assert_non_null(json);
    assert_string_equal(json, "{\"k\":42}");
    free(json);
}

static void test_propagates_backend_failure(void **state) {
    fixture_t *fx = *state;
    unsigned char dummy[32] = {0};
    assert_int_equal(write_creds_blob(fx, dummy, sizeof(dummy)), 0);

    will_return(__wrap_dbc_decrypt_aes_128_cbc, -1);

    char *json = get_dbeaver_credentials(nullptr);
    assert_null(json);
}

static void test_rejects_payload_shorter_than_prefix(void **state) {
    fixture_t *fx = *state;
    unsigned char dummy[32] = {0};
    assert_int_equal(write_creds_blob(fx, dummy, sizeof(dummy)), 0);

    static const unsigned char fake_plain[16] = {0};
    will_return(__wrap_dbc_decrypt_aes_128_cbc, 0);
    will_return(__wrap_dbc_decrypt_aes_128_cbc, fake_plain);
    will_return(__wrap_dbc_decrypt_aes_128_cbc, (size_t)16);

    char *json = get_dbeaver_credentials(nullptr);
    assert_null(json);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_strips_16_byte_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_propagates_backend_failure, setup, teardown),
        cmocka_unit_test_setup_teardown(test_rejects_payload_shorter_than_prefix, setup, teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
