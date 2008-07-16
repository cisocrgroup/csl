IF( CPPUNIT_FOUND )
 add_executable( testErrDic exec_testErrDic.cxx )
 target_link_libraries( testErrDic  csl ${CPPUNIT_LIBRARIES} )

 add_executable( testVaamAsErrDic exec_testVaamAsErrDic.cxx )
 target_link_libraries( testVaamAsErrDic  csl ${CPPUNIT_LIBRARIES} )

ENDIF( CPPUNIT_FOUND )
