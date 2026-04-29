if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  message(STATUS "Tests require GCC or Clang for -Wl,--wrap; skipping.")
  return()
endif()

find_package(cmocka REQUIRED)

if(TARGET cmocka::cmocka)
  set(cmocka_target cmocka::cmocka)
else()
  set(cmocka_target cmocka)
endif()

if(DBEAVER_CREDS_BACKEND STREQUAL "native")
  if(APPLE)
    set(BACKEND_SOURCE ${CMAKE_SOURCE_DIR}/src/macos.c)
  elseif(WIN32)
    set(BACKEND_SOURCE ${CMAKE_SOURCE_DIR}/src/win32.c)
  endif()
else()
  set(BACKEND_SOURCE ${CMAKE_SOURCE_DIR}/src/openssl.c)
endif()

set(test_include_dirs ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
set(test_compile_options -Wno-unused-parameter -Wno-unused-function)
set(test_compile_definitions
    _POSIX_C_SOURCE=200809L $<$<PLATFORM_ID:Darwin>:_DARWIN_C_SOURCE>)

function(dbc_configure_test target)
  target_include_directories(${target} PRIVATE ${test_include_dirs})
  target_compile_options(${target} PRIVATE ${test_compile_options})
  target_compile_definitions(${target} PRIVATE ${test_compile_definitions})
  target_link_libraries(${target} PRIVATE ${cmocka_target})
  if(DBEAVER_CREDS_BACKEND STREQUAL "openssl")
    target_link_libraries(${target} PRIVATE OpenSSL::Crypto)
  elseif(WIN32)
    target_link_libraries(${target} PRIVATE bcrypt)
  endif()
  add_asan(${target})
  add_ubsan(${target})
  add_coverage(${target})
endfunction()

# test_backend: real crypto, no mocks.
add_executable(test_backend test_backend.c ${BACKEND_SOURCE})
dbc_configure_test(test_backend)
add_test(NAME backend COMMAND test_backend)

# test_api: real backend + real env + real temp file.
add_executable(test_api test_api.c ${CMAKE_SOURCE_DIR}/src/api.c ${BACKEND_SOURCE})
dbc_configure_test(test_api)
add_test(NAME api COMMAND test_api)

# test_api_mock: mocked backend via -Wl,--wrap=dbc_decrypt_aes_128_cbc.
# Apple's ld64 does not implement --wrap, so the target is gated on non-Apple.
if(NOT APPLE)
  add_executable(test_api_mock
    test_api_mock.c
    wrappers.c
    ${CMAKE_SOURCE_DIR}/src/api.c
    ${BACKEND_SOURCE})
  dbc_configure_test(test_api_mock)
  target_link_options(test_api_mock PRIVATE -Wl,--wrap=dbc_decrypt_aes_128_cbc)
  add_test(NAME api_mock COMMAND test_api_mock)
endif()
