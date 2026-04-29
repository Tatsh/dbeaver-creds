find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

Python_add_library(_dbeaver_creds MODULE WITH_SOABI _dbeaver_creds.c)
target_link_libraries(_dbeaver_creds PRIVATE dbeaver_creds_core)

install(TARGETS _dbeaver_creds LIBRARY DESTINATION dbeaver_creds)
