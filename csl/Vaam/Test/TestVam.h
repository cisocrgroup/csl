#ifndef CSL_TEST_VAM_H
#define CSL_TEST_VAM_H CSL_TEST_VAM_H

#include "../../Global.h"
#include "../Vam.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVam : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVam );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testPatternGraph );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVam();
	void testBasics();
	void testPatternGraph();
// 	void testConstructionDetails();
// 	void lookupAllKeys();

    private:
	
    }; // class TestVam

    CPPUNIT_TEST_SUITE_REGISTRATION( TestVam );

    TestVam::TestVam() :
	CppUnit::TestFixture() {
	
    }

    void TestVam::testPatternGraph() {
	PatternGraph pg;
	pg.loadPatterns( ( Global::CSLROOT + "Vam/Test/testPatterns.txt" ).c_str() );

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
    void TestVam::testBasics() {
	MinDic<> dic;
	dic.loadFromFile( ( Global::CSLROOT + "Vam/Test/smallDic.mdic" ).c_str() );
	Vam vam( dic, ( Global::CSLROOT + "Vam/Test/testPatterns.txt" ).c_str() );
	
    }


} // namespace csl


#endif
