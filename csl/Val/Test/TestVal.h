#ifndef CSL_TEST_VAL_H
#define CSL_TEST_VAL_H CSL_TEST_VAL_H

#include "../../Global.h"
#include "../Val.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVal : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVal );
	CPPUNIT_TEST( testPatternGraph );
//	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVal();
	void testBasics();
	void testPatternGraph();
// 	void testConstructionDetails();
// 	void lookupAllKeys();
	
    private:
	std::string path_;

    }; // class TestVal

    CPPUNIT_TEST_SUITE_REGISTRATION( TestVal );

    TestVal::TestVal() :
	CppUnit::TestFixture() {

    }

    void TestVal::testPatternGraph() {

	PatternGraph pg;
	pg.loadPatterns( "../csl/Val/Test/small.patterns.txt" );

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

    void TestVal::testBasics() {
	MinDic<> baseDic;
	baseDic.initConstruction();
	baseDic.addToken( L"anna", 42 );
	baseDic.addToken( L"teil", 42 );
	baseDic.finishConstruction();

 	Val val( baseDic,  "../csl/Val/Test/small.patterns.txt" );


    }


//     /**
//      * test the basic methods for reading access like getRoot, walk, isFinal etc.
//      */
//     void TestVal::testBasics() {
//  	MinDic<> baseDic( "../csl/Val/Test/small.base.mdic" );
// 	MinDic<> filterDic( "../csl/Val/Test/small.filter.mdic" );
// 	Val<> val( baseDic,  "../csl/Val/Test/small.patterns.txt" );
	
// 	Val<>::CandidateReceiver answers;

// 	// a standard variant
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"xachen" ), &answers ) );
// 	CPPUNIT_ASSERT( answers.size() == 1 );
// 	Interpretation& answer = answers.at( 0 ); 
// 	CPPUNIT_ASSERT( answer.getBaseWord() == L"aachen" );
// 	CPPUNIT_ASSERT( answer.getInstruction().size() == 1 );
// 	CPPUNIT_ASSERT( answer.getInstruction().at( 0 ).getLeft() == L"a" );
// 	CPPUNIT_ASSERT( answer.getInstruction().at( 0 ).getRight() == L"x" );

// 	answers.clear();

// 	///////////// MAX NR OF PATTERNS ///////////////////////
// 	// with at most 0 variants, this should be no variant
// 	val.setMaxNrOfPatterns( 0 );
// 	CPPUNIT_ASSERT( ! val.query( std::wstring( L"xachen" ), &answers ) );
// 	// with at most 1 variant, this should work again
// 	val.setMaxNrOfPatterns( 1 );
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"xachen" ), &answers ) );
// 	// but not a word with 2 patterns
// 	CPPUNIT_ASSERT( ! val.query( std::wstring( L"kleintheyle" ), &answers ) );

// 	// switch back to an infinite nr of patterns
// 	val.setMaxNrOfPatterns( Val<>::INFINITE );
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"kleintheyle" ), &answers ) );


// 	///////////// MIN NR OF PATTERNS ///////////////////////

// 	// default should be 0
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"aachen" ), &answers ) );

// 	val.setMinNrOfPatterns( 1 );
// 	CPPUNIT_ASSERT( ! val.query( std::wstring( L"aachen" ), &answers ) );
	
// 	val.setMinNrOfPatterns( 2 );
// 	CPPUNIT_ASSERT( ! val.query( std::wstring( L"xachen" ), &answers ) );
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"xxchen" ), &answers ) );
	
// 	val.setMinNrOfPatterns( 0 );


// 	///////// FILTER DIC ///////////////////////////////

// 	// without the filterDic_ this should be a variant
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"hanne" ), &answers ) );

// 	// now it should be filtered
// 	val.setFilterDic( filterDic );
// 	CPPUNIT_ASSERT( ! val.query( std::wstring( L"hanne" ), &answers ) );

//     }

} // namespace csl


#endif
