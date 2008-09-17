#ifndef CSL_TEST_VAAM_H
#define CSL_TEST_VAAM_H CSL_TEST_VAAM_H

#include "../../Global.h"
#include "../Vaam.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVaam : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVaam );
	CPPUNIT_TEST( testPatternGraph );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVaam();
	void testBasics();
	void testPatternGraph();
// 	void testConstructionDetails();
// 	void lookupAllKeys();
	
    private:
	std::string path_;

    }; // class TestVaam

    CPPUNIT_TEST_SUITE_REGISTRATION( TestVaam );

    TestVaam::TestVaam() :
	CppUnit::TestFixture() {

    }

    void TestVaam::testPatternGraph() {

	PatternGraph pg;
	pg.loadPatterns( "../csl/Vaam/Test/small.patterns.txt" );

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
 	MinDic<> baseDic( "../csl/Vaam/Test/small.base.mdic" );
	MinDic<> filterDic( "../csl/Vaam/Test/small.filter.mdic" );
	Vaam<> vaam( baseDic,  "../csl/Vaam/Test/small.patterns.txt" );
	
	Vaam<>::CandidateReceiver answers;

	// a standard variant
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"xachen" ), &answers ) );

	std::wcout<<__FILE__<<":"<<__LINE__<<std::endl;

	// without the filterDic_ this should be a variant
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"hanne" ), &answers ) );
	CPPUNIT_ASSERT( answers.size() == 1 );
	Interpretation& answer = answers.at( 0 ); 
	CPPUNIT_ASSERT( answer.getBaseWord() == L"kanne" );
	CPPUNIT_ASSERT( answer.instruction.size() == 1 );
	CPPUNIT_ASSERT( answer.instruction.at( 0 ).getLeft() == L"k" );
	CPPUNIT_ASSERT( answer.instruction.at( 0 ).getRight() == L"h" );

	std::wcout<<__FILE__<<":"<<__LINE__<<std::endl;

	// now it should be filtered
	vaam.setFilterDic( filterDic );
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"hanne" ), &answers ) );

    }

} // namespace csl


#endif
