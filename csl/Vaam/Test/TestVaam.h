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
	pg.loadPatterns( ( Global::cslRootDir + "csl/Vaam/Test/small.patterns.txt" ).c_str() );

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
	MinDic<> baseDic( ( Global::cslRootDir + "csl/Vaam/Test/small.base.mdic" ).c_str() );
	MinDic<> filterDic( ( Global::cslRootDir + "csl/Vaam/Test/small.filter.mdic" ).c_str() );
	Vaam vaam( baseDic, ( Global::cslRootDir + "csl/Vaam/Test/small.patterns.txt" ).c_str() );

	std::vector< Interpretation > answers;

	// a standard variant
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"xachen" ), &answers ) );

	// without the filterDic_ this should be a variant
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"hanne" ), &answers ) );

	// now it should be filtered
	vaam.setFilterDic( filterDic );
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"hanne" ), &answers ) );

    }

} // namespace csl


#endif
