#ifndef DBEAVER_CREDS_H
#define DBEAVER_CREDS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error codes written through the @c err out-parameter of
 * ::get_dbeaver_credentials.
 */
enum dbeaver_credentials_error {
    DBEAVER_CREDENTIALS_OK = 0,
    /**< Operation succeeded. */
    DBEAVER_CREDENTIALS_PATH_UNAVAILABLE,
    /**< The platform-default credentials path could not be determined. */
    DBEAVER_CREDENTIALS_FILE_READ_FAILED,
    /**< The credentials file could not be opened or fully read. */
    DBEAVER_CREDENTIALS_INVALID_CIPHERTEXT,
    /**< The ciphertext was empty or not a multiple of the AES block size. */
    DBEAVER_CREDENTIALS_DECRYPTION_FAILED,
    /**< The crypto backend rejected the ciphertext during decryption. */
    DBEAVER_CREDENTIALS_EMPTY_PAYLOAD,
    /**< Decryption produced no JSON payload after the IV prefix. */
    DBEAVER_CREDENTIALS_OUT_OF_MEMORY,
    /**< A heap allocation failed while assembling the result. */
};

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
 *
 * @param[in]  path ``const char *`` - Filesystem path to the credentials
 *                  file, or NULL to use the platform-default location.
 * @param[out] err  ``enum dbeaver_credentials_error *`` - Receives the result
 *                  code (::DBEAVER_CREDENTIALS_OK on success, otherwise one
 *                  of the failure codes). May be NULL to ignore.
 * @return ``char *`` - Newly allocated, NUL-terminated UTF-8 JSON string on
 *         success, or NULL on failure.
 */
char *get_dbeaver_credentials(const char *path, enum dbeaver_credentials_error *err);

#ifdef __cplusplus
}
#endif

#endif
