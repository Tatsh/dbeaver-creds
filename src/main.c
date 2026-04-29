#include "compat.h"

#include <stdio.h>
#include <stdlib.h>

#include <dbeaver-creds.h>

int main(int argc, char *argv[]) {
    const char *path = nullptr;
    if (argc > 1) {
        path = argv[1];
    }
    char *json = get_dbeaver_credentials(path);
    if (!json) {
        return 1;
    }
    fputs(json, stdout);
    putchar('\n');
    free(json);
    return 0;
}
