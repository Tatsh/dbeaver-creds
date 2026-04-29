#include "compat.h"

#include <stdio.h>
#include <stdlib.h>

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
