#ifndef CSL_TEST_VAM_H
#define CSL_TEST_VAM_H CSL_TEST_VAM_H

#include "../../Global.h"
#include "../Vaam.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVaam : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVaam );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testPatternGraph );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVaam();
	void testBasics();
	void testPatternGraph();
// 	void testConstructionDetails();
// 	void lookupAllKeys();

    private:
	
    }; // class TestVaam

    CPPUNIT_TEST_SUITE_REGISTRATION( TestVaam );

    TestVaam::TestVaam() :
	CppUnit::TestFixture() {
	
    }

    void TestVaam::testPatternGraph() {
	PatternGraph pg;
	pg.loadPatterns( ( Global::CSLROOT + "Vaam/Test/testPatterns.txt" ).c_str() );

	// implicit copy constructor
	PatternGraph::State st = pg.getRoot();
	PatternGraph::State st2 = pg.getRoot();

	// == operator
	CPPUNIT_ASSERT( st == st2 );

	// basic walk()-operation and isFinal()
	CPPUNIT_ASSERT( ! st.isFinal() );
	CPPUNIT_ASSERT( st.walk( 't' ) );
	CPPUNIT_ASSERT( st.isFinal() );

	st = pg.getRoot();
	PatternGraph::State st3( st );
	st2 = st.getTransTarget( 't' );
	CPPUNIT_ASSERT( st == pg.getRoot() );
	
    }

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestVaam::testBasics() {
	MinDic<> dic;
	dic.loadFromFile( ( Global::CSLROOT + "Vaam/Test/smallDic.mdic" ).c_str() );
	Vaam vam( dic, ( Global::CSLROOT + "Vaam/Test/testPatterns.txt" ).c_str() );
	
    }


} // namespace csl


#endif
