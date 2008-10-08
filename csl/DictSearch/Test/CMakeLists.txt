IF( CPPUNIT_FOUND )
add_executable( testDictSearch exec_testDictSearch.cxx )
target_link_libraries( testDictSearch ${CPPUNIT_LIBRARIES} csl )
ENDIF( CPPUNIT_FOUND )
