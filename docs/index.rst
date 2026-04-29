dbeaver-creds
=============================

.. include:: badges.rst

Decrypt DBeaver's credentials-config.json file and display the output (a JSON string).

This project ships in six flavours so you can use it in whichever language or
shell suits you:

- **Python module** (``pip install dbeaver-creds``) - importable as
  ``dbeaver_creds`` and exposes a ``dbeaver-creds`` console script. C-extension
  under the hood, no external dependencies on your machine.
- **Native CLI binary** built from this source tree via CMake. Single static
  binary, no runtime dependencies beyond the chosen crypto backend
  (CommonCrypto on macOS, BCrypt on Windows, OpenSSL on Linux).
- **C library** (``#include <dbeaver-creds.h>``) - link
  ``dbeaver_creds_core.a`` and call ``get_dbeaver_credentials(path)``.
- **Bash script** that works on Linux, macOS, and Windows. Requires
  ``openssl`` and ``dd`` to be in ``PATH``.
- **PowerShell module** exposing ``Show-DBeaver-Credential-Json`` (alias
  ``dbeaver-creds``). Pure .NET; no external dependencies.
- **Batch script** (Windows only) equivalent to the Bash script with the same
  ``openssl`` + ``dd`` requirements.

Installation
------------

Python
~~~~~~

.. code-block:: shell

   pip install dbeaver-creds

Native CLI / C library
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   sudo cmake --install build

``DBEAVER_CREDS_BACKEND=native|openssl`` (passed via ``-D``) overrides the auto-detected backend.

PowerShell
~~~~~~~~~~

.. code-block:: powershell

   Install-Module -Name DBeaverCreds

Usage
-----

All entry points accept an optional path argument; if absent, the
platform-default credentials path is used.

Native CLI / Bash / Batch
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

   dbeaver-creds [PATH]

Python
~~~~~~

.. code-block:: python

   from dbeaver_creds import get_dbeaver_credentials

   print(get_dbeaver_credentials())                    # default path
   print(get_dbeaver_credentials('/some/where.json'))  # explicit

C library
~~~~~~~~~

.. code-block:: c

   #include <dbeaver-creds.h>
   #include <stdio.h>
   #include <stdlib.h>

   int main(void) {
     enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
     char *json = get_dbeaver_credentials(NULL, &err);
     if (!json) {
       fprintf(stderr, "dbeaver-creds failed: %d\n", err);
       return 1;
     }
     puts(json);
     free(json);
     return 0;
   }

PowerShell
~~~~~~~~~~

.. code-block:: powershell

   Show-DBeaver-Credential-Json
   Show-DBeaver-Credential-Json -Path 'C:\custom\credentials-config.json'


.. only:: html

   .. automodule:: dbeaver_creds
      :members:

   Indices and tables
   ==================
   * :ref:`genindex`
   * :ref:`modindex`
