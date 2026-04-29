#ifndef DBEAVER_CREDS_H
#define DBEAVER_CREDS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read DBeaver's credentials-config.json, decrypt it, and return the JSON
 * payload as a malloc-allocated, NUL-terminated UTF-8 string.
 *
 * If @p path is NULL, the platform-default location is used:
 *   - Linux: ``$XDG_DATA_HOME/DBeaverData/...`` (or ``$HOME/.local/share/...``)
 *   - macOS: ``$HOME/Library/DBeaverData/...``
 *   - Windows: ``%APPDATA%\\DBeaverData\\...``
 *
 * The caller owns the returned buffer and must release it with free().
 * Returns NULL on any failure; a diagnostic is written to stderr.
 */
char *get_dbeaver_credentials(const char *path);

#ifdef __cplusplus
}
#endif

#endif
