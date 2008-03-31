include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(vaamFilter vaamFilter.cxx)
target_link_libraries (vaamFilter csl)
