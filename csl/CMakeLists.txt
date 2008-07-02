INCLUDE(CPack)

add_subdirectory(MinDic)
add_subdirectory(FBDic)
add_subdirectory(ErrDic)
add_subdirectory(MinDicString)
add_subdirectory(Trie)

ADD_LIBRARY(csl STATIC Alphabet/Alphabet.cxx LevDEA/LevDEA.cxx Trie/Trie.cxx)

install( 
  TARGETS csl
  DESTINATION lib
  )

INSTALL(DIRECTORY . DESTINATION include/csl/
 PATTERN "*svn*" EXCLUDE
 PATTERN "*CMake*" EXCLUDE
 )

