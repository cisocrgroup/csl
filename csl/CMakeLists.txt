INCLUDE(CPack)

add_subdirectory(Alphabet)
add_subdirectory(MinDic)
add_subdirectory(FBDic)
add_subdirectory(LevDEA)
add_subdirectory(ErrDic)
add_subdirectory(MinDicString)
add_subdirectory(Vaam)
add_subdirectory(SimpleFSA)
add_subdirectory(DictSearch)

ADD_LIBRARY( csl SHARED csl.cxx )
target_link_libraries( csl Alphabet )
target_link_libraries( csl LevDEA )

install( 
  TARGETS csl
  DESTINATION lib
  )

INSTALL(DIRECTORY . DESTINATION include/csl/
 PATTERN "*svn*" EXCLUDE
 PATTERN "*CMake*" EXCLUDE
 )

