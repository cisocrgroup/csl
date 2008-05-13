
add_subdirectory(FBDic)
add_subdirectory(ErrDic)
add_subdirectory(MinDicString)
add_subdirectory(Trie)

ADD_LIBRARY(csl STATIC Alphabet/Alphabet.cxx LevDEA/LevDEA.cxx Trie/Trie.cxx)
