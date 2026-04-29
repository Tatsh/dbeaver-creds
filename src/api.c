#include "compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "win_utf8.h"
#endif

#include <dbeaver-creds.h>

#include "backend.h"

/* clang-format off */
static const unsigned char kKey[16] = {
    0xba, 0xbb, 0x4a, 0x9f, 0x77, 0x4a, 0xb8, 0x53,
    0xc9, 0x6c, 0x2d, 0x65, 0x3d, 0xfe, 0x54, 0x4a
};
/* clang-format on */
static const unsigned char kIv[16] = {0};

#if defined(__APPLE__)
#define DBC_PATH_SUFFIX "/Library/DBeaverData/workspace6/General/.dbeaver/credentials-config.json"
#elif defined(_WIN32)
#define DBC_PATH_SUFFIX "\\DBeaverData\\workspace6\\General\\.dbeaver\\credentials-config.json"
#else
#define DBC_PATH_SUFFIX "/DBeaverData/workspace6/General/.dbeaver/credentials-config.json"
#endif

static char *find_config_path(void) {
#if defined(_WIN32)
    DWORD needed = GetEnvironmentVariableW(L"APPDATA", nullptr, 0);
    if (needed == 0) {
        return nullptr;
    }
    wchar_t *wbase = (wchar_t *)malloc((size_t)needed * sizeof(wchar_t));
    if (!wbase) {
        return nullptr; // LCOV_EXCL_LINE
    }
    if (GetEnvironmentVariableW(L"APPDATA", wbase, needed) != needed - 1) {
        // LCOV_EXCL_START
        free(wbase);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    char *base = dbc_utf16_to_utf8(wbase);
    free(wbase);
    if (!base) {
        return nullptr; // LCOV_EXCL_LINE
    }
    size_t n = strlen(base) + strlen(DBC_PATH_SUFFIX) + 1;
    char *path = (char *)malloc(n);
    if (!path) {
        // LCOV_EXCL_START
        free(base);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    snprintf(path, n, "%s%s", base, DBC_PATH_SUFFIX);
    free(base);
    return path;
#elif defined(__APPLE__)
    const char *home = getenv("HOME");
    if (!home || !*home) {
        return nullptr; // LCOV_EXCL_LINE
    }
    size_t n = strlen(home) + strlen(DBC_PATH_SUFFIX) + 1;
    char *path = (char *)malloc(n);
    if (!path) {
        return nullptr; // LCOV_EXCL_LINE
    }
    snprintf(path, n, "%s%s", home, DBC_PATH_SUFFIX);
    return path;
#else
    const char *xdg = getenv("XDG_DATA_HOME");
    const char *prefix;
    const char *infix;
    if (xdg && *xdg) {
        prefix = xdg;
        infix = "";
    } else {
        // LCOV_EXCL_START
        const char *home = getenv("HOME");
        if (!home || !*home) {
            return nullptr;
        }
        // LCOV_EXCL_STOP
        // LCOV_EXCL_START
        prefix = home;
        infix = "/.local/share";
        // LCOV_EXCL_STOP
    }
    size_t n = strlen(prefix) + strlen(infix) + strlen(DBC_PATH_SUFFIX) + 1;
    char *path = (char *)malloc(n);
    if (!path) {
        return nullptr; // LCOV_EXCL_LINE
    }
    snprintf(path, n, "%s%s%s", prefix, infix, DBC_PATH_SUFFIX);
    return path;
#endif
}

static int read_all(const char *path, unsigned char **out, size_t *out_len) {
#if defined(_WIN32)
    FILE *f = dbc_fopen_utf8(path, "rb");
#else
    FILE *f = fopen(path, "rb");
#endif
    if (!f) {
        return -1;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        // LCOV_EXCL_START
        fclose(f);
        return -1;
        // LCOV_EXCL_STOP
    }
    long sz = ftell(f);
    if (sz < 0) {
        // LCOV_EXCL_START
        fclose(f);
        return -1;
        // LCOV_EXCL_STOP
    }
    rewind(f);
    unsigned char *buf = (unsigned char *)malloc((size_t)sz);
    if (!buf) {
        // LCOV_EXCL_START
        fclose(f);
        return -1;
        // LCOV_EXCL_STOP
    }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        // LCOV_EXCL_START
        free(buf);
        fclose(f);
        return -1;
        // LCOV_EXCL_STOP
    }
    fclose(f);
    *out = buf;
    *out_len = (size_t)sz;
    return 0;
}

static void set_err(enum dbeaver_credentials_error *err, enum dbeaver_credentials_error code) {
    if (err) {
        *err = code;
    }
}

char *get_dbeaver_credentials(const char *path, enum dbeaver_credentials_error *err) {
    char *owned_path = nullptr;
    if (!path) {
        owned_path = find_config_path();
        if (!owned_path) {
            // LCOV_EXCL_START
            set_err(err, DBEAVER_CREDENTIALS_PATH_UNAVAILABLE);
            return nullptr;
            // LCOV_EXCL_STOP
        }
        path = owned_path;
    }
    unsigned char *cipher = nullptr;
    size_t cipher_len = 0;
    if (read_all(path, &cipher, &cipher_len) != 0) {
        set_err(err, DBEAVER_CREDENTIALS_FILE_READ_FAILED);
        free(owned_path);
        return nullptr;
    }
    free(owned_path);
    if (cipher_len == 0 || cipher_len % 16 != 0) {
        set_err(err, DBEAVER_CREDENTIALS_INVALID_CIPHERTEXT);
        free(cipher);
        return nullptr;
    }
    unsigned char *plain = (unsigned char *)malloc(cipher_len);
    if (!plain) {
        // LCOV_EXCL_START
        set_err(err, DBEAVER_CREDENTIALS_OUT_OF_MEMORY);
        free(cipher);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    size_t plain_len = cipher_len;
    if (dbc_decrypt_aes_128_cbc(kKey, kIv, cipher, cipher_len, plain, &plain_len) != 0) {
        set_err(err, DBEAVER_CREDENTIALS_DECRYPTION_FAILED);
        free(plain);
        free(cipher);
        return nullptr;
    }
    free(cipher);
    if (plain_len <= 16) {
        set_err(err, DBEAVER_CREDENTIALS_EMPTY_PAYLOAD);
        free(plain);
        return nullptr;
    }
    size_t json_len = plain_len - 16;
    char *out = (char *)malloc(json_len + 1);
    if (!out) {
        // LCOV_EXCL_START
        set_err(err, DBEAVER_CREDENTIALS_OUT_OF_MEMORY);
        free(plain);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    memcpy(out, plain + 16, json_len);
    out[json_len] = '\0';
    free(plain);
    set_err(err, DBEAVER_CREDENTIALS_OK);
    return out;
}
