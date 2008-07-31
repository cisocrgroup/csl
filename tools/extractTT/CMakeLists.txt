include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(extractTT extractTT.cxx)
target_link_libraries (extractTT csl)

