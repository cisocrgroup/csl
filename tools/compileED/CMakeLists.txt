include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(compileED compileED.cxx)
target_link_libraries (compileED csl)

install( 
  TARGETS compileED
  DESTINATION bin
  
  )
