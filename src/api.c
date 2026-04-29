#include "compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
    DWORD needed = GetEnvironmentVariableA("APPDATA", nullptr, 0);
    if (needed == 0) {
        return nullptr;
    }
    char *base = (char *)malloc(needed);
    if (!base) {
        return nullptr;
    }
    if (GetEnvironmentVariableA("APPDATA", base, needed) != needed - 1) {
        free(base);
        return nullptr;
    }
    size_t n = strlen(base) + strlen(DBC_PATH_SUFFIX) + 1;
    char *path = (char *)malloc(n);
    if (!path) {
        free(base);
        return nullptr;
    }
    snprintf(path, n, "%s%s", base, DBC_PATH_SUFFIX);
    free(base);
    return path;
#elif defined(__APPLE__)
    const char *home = getenv("HOME");
    if (!home || !*home) {
        return nullptr;
    }
    size_t n = strlen(home) + strlen(DBC_PATH_SUFFIX) + 1;
    char *path = (char *)malloc(n);
    if (!path) {
        return nullptr;
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
        const char *home = getenv("HOME");
        if (!home || !*home) {
            return nullptr;
        }
        prefix = home;
        infix = "/.local/share";
    }
    size_t n = strlen(prefix) + strlen(infix) + strlen(DBC_PATH_SUFFIX) + 1;
    char *path = (char *)malloc(n);
    if (!path) {
        return nullptr;
    }
    snprintf(path, n, "%s%s%s", prefix, infix, DBC_PATH_SUFFIX);
    return path;
#endif
}

static int read_all(const char *path, unsigned char **out, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return -1;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return -1;
    }
    long sz = ftell(f);
    if (sz < 0) {
        fclose(f);
        return -1;
    }
    rewind(f);
    unsigned char *buf = (unsigned char *)malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return -1;
    }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return -1;
    }
    fclose(f);
    *out = buf;
    *out_len = (size_t)sz;
    return 0;
}

char *get_dbeaver_credentials(const char *path) {
    char *owned_path = nullptr;
    if (!path) {
        owned_path = find_config_path();
        if (!owned_path) {
            fprintf(stderr, "Could not determine credentials path.\n");
            return nullptr;
        }
        path = owned_path;
    }
    unsigned char *cipher = nullptr;
    size_t cipher_len = 0;
    if (read_all(path, &cipher, &cipher_len) != 0) {
        fprintf(stderr, "credentials-config.json file not found or could not be read.\n");
        free(owned_path);
        return nullptr;
    }
    free(owned_path);
    if (cipher_len == 0 || cipher_len % 16 != 0) {
        fprintf(stderr, "Invalid ciphertext length.\n");
        free(cipher);
        return nullptr;
    }
    unsigned char *plain = (unsigned char *)malloc(cipher_len);
    if (!plain) {
        free(cipher);
        return nullptr;
    }
    size_t plain_len = cipher_len;
    if (dbc_decrypt_aes_128_cbc(kKey, kIv, cipher, cipher_len, plain, &plain_len) != 0) {
        fprintf(stderr, "Decryption failed.\n");
        free(plain);
        free(cipher);
        return nullptr;
    }
    free(cipher);
    if (plain_len <= 16) {
        fprintf(stderr, "Decryption produced no payload.\n");
        free(plain);
        return nullptr;
    }
    size_t json_len = plain_len - 16;
    char *out = (char *)malloc(json_len + 1);
    if (!out) {
        free(plain);
        return nullptr;
    }
    memcpy(out, plain + 16, json_len);
    out[json_len] = '\0';
    free(plain);
    return out;
}
