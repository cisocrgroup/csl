#SUBDIRS = Getopt Alphabet TransTable MinDic FBDic Hash LevFilter LevDEA ResultSet MSMatch Pattern Vam
add_subdirectory(Alphabet)


ADD_LIBRARY(csl SHARED Alphabet/Alphabet.cxx LevDEA/LevDEA.cxx)
