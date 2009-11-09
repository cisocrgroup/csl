INCLUDE(CPack)

add_subdirectory(Alphabet)
add_subdirectory(MinDic)
add_subdirectory(FBDic)
add_subdirectory(LevDEA)
add_subdirectory(ErrDic)
add_subdirectory(MinDicString)
add_subdirectory(MinDicFlexible)
add_subdirectory(Vaam)
add_subdirectory(SimpleFSA)
add_subdirectory(DictSearch)
add_subdirectory(Trie)
add_subdirectory(Pattern)
add_subdirectory(Locale)

ADD_LIBRARY( csl SHARED Alphabet/Alphabet.cxx )
TARGET_LINK_LIBRARIES( csl Alphabet )
TARGET_LINK_LIBRARIES( csl LevDEA )
TARGET_LINK_LIBRARIES( csl DictSearch )
target_link_libraries( csl Pattern )

install( 
  TARGETS csl
  DESTINATION lib
  )

INSTALL(DIRECTORY . DESTINATION include/csl/
 PATTERN "*svn*" EXCLUDE
 PATTERN "*CMake*" EXCLUDE
 )

