include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(vaamFilter vaamFilter.cxx)
target_link_libraries (vaamFilter csl)
install( 
  TARGETS vaamFilter
  DESTINATION bin
  
  )

add_executable(vaamFilter_TT vaamFilter.cxx)
target_link_libraries (vaamFilter_TT csl)
ADD_DEFINITIONS(-DCSL_VAAMFILTER_USE_TRANSTABLE )
install( 
  TARGETS vaamFilter_TT
  DESTINATION bin
  
  )

add_executable(vamFilter vamFilter.cxx)
target_link_libraries (vamFilter csl)
install( 
  TARGETS vamFilter
  DESTINATION bin
  
  )
