add_executable(valFilter valFilter.cxx)
target_link_libraries (valFilter csl)
install( 
  TARGETS valFilter
  DESTINATION bin
  
  )
