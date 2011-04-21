INCLUDE(CPack)

add_subdirectory(CSLLocale)
add_subdirectory(Getopt)
add_subdirectory(Alphabet)
add_subdirectory(MinDic)
add_subdirectory(FBDic)
add_subdirectory(FBDicString)
add_subdirectory(LevDEA)
add_subdirectory(ErrDic)
add_subdirectory(MinDicString)
add_subdirectory(MinDicFlexible)
add_subdirectory(Vaam)
add_subdirectory(Val)
add_subdirectory(SimpleFSA)
add_subdirectory(DictSearch)
#add_subdirectory(Trie)
add_subdirectory(Pattern)
add_subdirectory(Locale)
add_subdirectory(INIConfig)

ADD_LIBRARY( csl SHARED 
  CSLLocale/CSLLocale.cxx
  Alphabet/Alphabet.cxx 
  LevDEA/LevDEA.cxx
  DictSearch/DictSearch.cxx
  Pattern/Pattern.cxx
  Pattern/ComputeInstruction.cxx
  Pattern/Instruction.cxx
  Pattern/Interpretation.cxx
  Pattern/PatternWeights.cxx
  INIConfig/INIConfig.cxx INIConfig/INIParser.cxx INIConfig/dictionary.c
  )

#TARGET_LINK_LIBRARIES( csl Alphabet )
#TARGET_LINK_LIBRARIES( csl LevDEA )
#TARGET_LINK_LIBRARIES( csl DictSearch )
#TARGET_LINK_LIBRARIES( csl Pattern )
#TARGET_LINK_LIBRARIES( csl DictSearch )

install( 
  TARGETS csl
  DESTINATION lib
  )

INSTALL(DIRECTORY . DESTINATION include/csl/
 PATTERN "*svn*" EXCLUDE
 PATTERN "*CMake*" EXCLUDE
 )

