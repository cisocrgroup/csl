include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(vaamFilter vaamFilter.cxx)
target_link_libraries (vaamFilter csl)
install( 
  TARGETS vaamFilter
  DESTINATION bin
  
  )

add_executable(vamFilter vamFilter.cxx)
target_link_libraries (vamFilter csl)
install( 
  TARGETS vamFilter
  DESTINATION bin
  
  )
