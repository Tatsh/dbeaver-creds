#ifndef DBC_WIN_UTF8_H
#define DBC_WIN_UTF8_H

#if defined(_WIN32)

#include <stdio.h>
#include <wchar.h>

/* Convert a UTF-16 string to a freshly malloc'd UTF-8 string.
 * Caller frees with free(). Returns NULL on conversion or allocation
 * failure. */
char *dbc_utf16_to_utf8(const wchar_t *src);

/* Convert a UTF-8 string to a freshly malloc'd UTF-16 string.
 * Caller frees with free(). Returns NULL on conversion or allocation
 * failure. */
wchar_t *dbc_utf8_to_utf16(const char *src);

/* fopen wrapper that treats path and mode as UTF-8 (uses _wfopen under
 * the hood). Returns NULL on conversion or open failure. */
FILE *dbc_fopen_utf8(const char *path, const char *mode);

#endif

#endif
