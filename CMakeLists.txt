include(CheckTypeSize)

cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(dbeaver-creds
        VERSION 0.0.3
        LANGUAGES C
        DESCRIPTION "Decrypt DBeaver's credentials-config.json file."
        HOMEPAGE_URL "https://github.com/Tatsh/dbeaver-creds")
string(TIMESTAMP DBC_COPYRIGHT_YEAR "%Y" UTC)

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  set(CMAKE_C_STANDARD 23)
else()
  set(CMAKE_C_STANDARD 11)
endif()
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(BUILD_SHARED_LIBS OFF)

option(BUILD_CLI "Build the dbeaver-creds CLI executable." ON)
option(BUILD_DOCS "Build Doxygen documentation for the public header." OFF)
option(BUILD_PYTHON_EXTENSION "Build the Python C extension module." OFF)
option(BUILD_TESTS "Build cmocka tests." OFF)
option(ENABLE_ASAN "Enable address sanitiser on test targets." OFF)
option(ENABLE_COVERAGE "Compile test targets with coverage instrumentation." OFF)
option(ENABLE_UBSAN "Enable undefined behaviour sanitiser on test targets." OFF)

function(add_asan target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${target} PRIVATE $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
    target_link_options(${target} PRIVATE $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
  endif()
endfunction()

function(add_ubsan target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${target} PRIVATE $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>)
    target_link_options(${target} PRIVATE $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>)
  endif()
endfunction()

function(add_coverage target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${target} PRIVATE $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>)
    target_link_options(${target} PRIVATE $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>)
  endif()
endfunction()

set(DBEAVER_CREDS_BACKEND ""
    CACHE STRING
    "Crypto backend: 'native' (CommonCrypto/BCrypt) or 'openssl'. Empty = auto-detect.")
set_property(CACHE DBEAVER_CREDS_BACKEND PROPERTY STRINGS "" "native" "openssl")

if(DBEAVER_CREDS_BACKEND STREQUAL "")
  if(APPLE OR WIN32)
    set(DBEAVER_CREDS_BACKEND "native")
  else()
    set(DBEAVER_CREDS_BACKEND "openssl")
  endif()
  message(STATUS "DBEAVER_CREDS_BACKEND not set; using auto-detected '${DBEAVER_CREDS_BACKEND}'.")
endif()

if(NOT MSVC)
  set(CMAKE_REQUIRED_FLAGS "-std=gnu2x")
endif()
set(CMAKE_EXTRA_INCLUDE_FILES stddef.h)
check_type_size(nullptr_t STD_NULLPTR_T BUILTIN_TYPES_ONLY LANGUAGE C)
unset(CMAKE_EXTRA_INCLUDE_FILES)
add_compile_definitions(STD_NULLPTR_T=$<IF:$<BOOL:${STD_NULLPTR_T}>,1,0>)

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(DBEAVER_CREDS_BACKEND STREQUAL "openssl")
  find_package(OpenSSL REQUIRED)
endif()

add_subdirectory(src)

if(BUILD_PYTHON_EXTENSION)
  add_subdirectory(dbeaver_creds)
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(NOT SKBUILD)
  install(FILES include/dbeaver-creds.h DESTINATION include)
  install(FILES man/dbeaver-creds.1 DESTINATION man/man1)
  install(FILES man/dbeaver-creds.h.3 DESTINATION man/man3 OPTIONAL)
  install(FILES LICENSE.txt README.md DESTINATION share/dbeaver-creds)

  set(CPACK_PACKAGE_NAME "dbeaver-creds")
  set(CPACK_PACKAGE_VENDOR "Tatsh")
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
  set(CPACK_PACKAGING_INSTALL_PREFIX "")
  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
  set(CPACK_THREADS 0)
  if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    # NSIS pre-include block: requests user-level execution (no admin elevation
    # prompt) and stamps the setup .exe with the same Windows version metadata
    # the CLI .exe carries via src/version.rc.in.
    set(CPACK_NSIS_DEFINES "RequestExecutionLevel user
VIProductVersion \\\"${PROJECT_VERSION}.0\\\"
VIAddVersionKey \\\"ProductName\\\" \\\"${CPACK_PACKAGE_NAME}\\\"
VIAddVersionKey \\\"CompanyName\\\" \\\"${CPACK_PACKAGE_VENDOR}\\\"
VIAddVersionKey \\\"FileDescription\\\" \\\"${CPACK_PACKAGE_NAME} installer.\\\"
VIAddVersionKey \\\"FileVersion\\\" \\\"${PROJECT_VERSION}\\\"
VIAddVersionKey \\\"ProductVersion\\\" \\\"${PROJECT_VERSION}.0\\\"
VIAddVersionKey \\\"LegalCopyright\\\" \\\"Copyright (c) ${DBC_COPYRIGHT_YEAR} ${CPACK_PACKAGE_VENDOR}.\\\"
VIAddVersionKey \\\"Comments\\\" \\\"${PROJECT_HOMEPAGE_URL}\\\"")
    set(CPACK_NSIS_INSTALL_ROOT "$COMMONPROGRAMDATA")
    set(CPACK_NSIS_CONTACT "https://bsky.app/profile/tatsh.bsky.social")
    set(CPACK_NSIS_MANIFEST_DPI_AWARE ON)
    set(CPACK_NSIS_URL_INFO_ABOUT "${PROJECT_HOMEPAGE_URL}/issues")
    set(CPACK_NSIS_MODIFY_PATH ON)
  else()
    set(CPACK_GENERATOR "ZIP")
  endif()
  string(TOLOWER "${CMAKE_SYSTEM_NAME}" _dbc_sys)
  string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _dbc_proc)
  if(_dbc_sys STREQUAL "darwin")
    set(_dbc_os_label "macos-${_dbc_proc}")
  else()
    set(_dbc_os_label "${_dbc_sys}-${_dbc_proc}")
  endif()
  set(CPACK_PACKAGE_FILE_NAME
      "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${_dbc_os_label}")
  include(CPack)
endif()

if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(docs
    COMMAND Doxygen::doxygen ${CMAKE_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating Doxygen documentation"
    VERBATIM)
endif()
