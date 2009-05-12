add_subdirectory( Test )

add_executable( createED createED.cxx )
target_link_libraries( createED csl )

add_executable( extractED extractED.cxx )
target_link_libraries( extractED csl )

add_executable( filterOldED filterOldED.cxx )
target_link_libraries( filterOldED csl )

install( 
  TARGETS createED
  DESTINATION bin
  
  )
