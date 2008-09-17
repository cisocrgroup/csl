include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(lookupED lookupED.cxx)
target_link_libraries (lookupED csl)

install( 
  TARGETS lookupED
  DESTINATION bin
  )
