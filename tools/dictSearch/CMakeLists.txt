include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(dictSearch dictSearch.cxx)
target_link_libraries (dictSearch csl)

install( 
  TARGETS dictSearch
  DESTINATION bin
  )
