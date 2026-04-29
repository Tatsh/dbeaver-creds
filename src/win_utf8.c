#include "compat.h"

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>

#include "win_utf8.h"

char *dbc_utf16_to_utf8(const wchar_t *src) {
    if (!src) {
        return nullptr; // LCOV_EXCL_LINE
    }
    int n = WideCharToMultiByte(CP_UTF8, 0, src, -1, nullptr, 0, nullptr, nullptr);
    if (n <= 0) {
        return nullptr; // LCOV_EXCL_LINE
    }
    char *dst = (char *)malloc((size_t)n);
    if (!dst) {
        return nullptr; // LCOV_EXCL_LINE
    }
    if (WideCharToMultiByte(CP_UTF8, 0, src, -1, dst, n, nullptr, nullptr) <= 0) {
        // LCOV_EXCL_START
        free(dst);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    return dst;
}

wchar_t *dbc_utf8_to_utf16(const char *src) {
    if (!src) {
        return nullptr; // LCOV_EXCL_LINE
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, src, -1, nullptr, 0);
    if (n <= 0) {
        return nullptr; // LCOV_EXCL_LINE
    }
    wchar_t *dst = (wchar_t *)malloc((size_t)n * sizeof(wchar_t));
    if (!dst) {
        return nullptr; // LCOV_EXCL_LINE
    }
    if (MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, n) <= 0) {
        // LCOV_EXCL_START
        free(dst);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    return dst;
}

FILE *dbc_fopen_utf8(const char *path, const char *mode) {
    wchar_t *wpath = dbc_utf8_to_utf16(path);
    if (!wpath) {
        return nullptr; // LCOV_EXCL_LINE
    }
    wchar_t *wmode = dbc_utf8_to_utf16(mode);
    if (!wmode) {
        // LCOV_EXCL_START
        free(wpath);
        return nullptr;
        // LCOV_EXCL_STOP
    }
    FILE *f = _wfopen(wpath, wmode);
    free(wpath);
    free(wmode);
    return f;
}

#endif
