#include "compat.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <shellapi.h>

#include "win_utf8.h"
#endif

#include <dbeaver-creds.h>

static const char *err_message(enum dbeaver_credentials_error err) {
    switch (err) {
    case DBEAVER_CREDENTIALS_OK:
        return "Success.";
    case DBEAVER_CREDENTIALS_PATH_UNAVAILABLE:
        return "Could not determine credentials path.";
    case DBEAVER_CREDENTIALS_FILE_READ_FAILED:
        return "credentials-config.json file not found or could not be read.";
    case DBEAVER_CREDENTIALS_INVALID_CIPHERTEXT:
        return "Invalid ciphertext length.";
    case DBEAVER_CREDENTIALS_DECRYPTION_FAILED:
        return "Decryption failed.";
    case DBEAVER_CREDENTIALS_EMPTY_PAYLOAD:
        return "Decryption produced no payload.";
    case DBEAVER_CREDENTIALS_OUT_OF_MEMORY:
        return "Out of memory.";
    }
    return "Unknown error."; // LCOV_EXCL_LINE
}

#if defined(_WIN32)
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    SetConsoleOutputCP(CP_UTF8);
    int wargc = 0;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    char *path_buf = nullptr;
    const char *path = nullptr;
    if (wargv && wargc > 1) {
        path_buf = dbc_utf16_to_utf8(wargv[1]);
        path = path_buf;
    }
    if (wargv) {
        LocalFree(wargv);
    }
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
    char *json = get_dbeaver_credentials(path, &err);
    free(path_buf);
    if (!json) {
        fprintf(stderr, "%s\n", err_message(err));
        return 1;
    }
    fputs(json, stdout);
    putchar('\n');
    free(json);
    return 0;
}
#else
int main(int argc, char *argv[]) {
    const char *path = nullptr;
    if (argc > 1) {
        path = argv[1];
    }
    enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
    char *json = get_dbeaver_credentials(path, &err);
    if (!json) {
        fprintf(stderr, "%s\n", err_message(err));
        return 1;
    }
    fputs(json, stdout);
    putchar('\n');
    free(json);
    return 0;
}
#endif
