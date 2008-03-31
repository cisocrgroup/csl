include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(msFilter msFilter.cxx)
target_link_libraries (msFilter csl)

