#ifndef CSL_TEST_DICTSEARCH_H
#define CSL_TEST_DICTSEARCH_H CSL_TEST_DICTSEARCH_H

#include <cppunit/extensions/HelperMacros.h>


#include"../DictSearch.h"

#include"../StringDictModule.h"

namespace csl {
    class TestDictSearch : public CppUnit::TestFixture {
    public:

	CPPUNIT_TEST_SUITE( TestDictSearch );
//	CPPUNIT_TEST( testDLev );
//	CPPUNIT_TEST( smokeTest );
	CPPUNIT_TEST( testCaseMode );
	CPPUNIT_TEST( testStringDictModule );
	CPPUNIT_TEST_SUITE_END();

	
    public:
	void smokeTest() {

	    // add a modern dict for exact matching, dlev==0
	    DictSearch ds;

	    
	    
	    DictSearch::CandidateSet result;

	    // lookup in modern dict with dlev 0
//	    ds.setModern( "../csl/DictSearch/Test/small.modern.fbdic", 0 );

	    DictSearch::DictModule& modernDict = ds.addDictModule( L"modern", std::string( "../csl/DictSearch/Test/small.modern.fbdic" ) );
	    ds.query( L"teile", &result );
	    CPPUNIT_ASSERT( result.size() == 1 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"teile"  );
	    CPPUNIT_ASSERT( result.at( 0 ).getInstruction().empty() );
	    

	    // change dlev to 1, 2
	    result.clear();
	    modernDict.setDLev( 1 );
	    ds.query( L"teile", &result );
	    CPPUNIT_ASSERT( result.size() == 3 );

	    modernDict.setDLev( 2 );
	    result.clear();
	    ds.query( L"teile", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 4 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"teile"  );
	    CPPUNIT_ASSERT( result.at( 1 ).getWord() == L"feile"  );
	    CPPUNIT_ASSERT( result.at( 2 ).getWord() == L"teilen"  );
	    CPPUNIT_ASSERT( result.at( 3 ).getWord() == L"teller"  );

	    // init the hpothetic dic, with dlev 0
	    ds.initHypothetic( "../csl/DictSearch/Test/small.patterns.txt" );
	    modernDict.setMaxNrOfPatterns( 1000 );
	    result.clear();
	    ds.query( L"theyle", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 2 );

	    // set dlev=2 for hypothetic dic
	    modernDict.setDLevHypothetic( 2 );
	    result.clear();
	    ds.query( L"teile", &result );
	    std::sort( result.begin(), result.end() );
	    size_t n = 0;
// 	    for( DictSearch::CandidateSet::const_iterator it = result.begin(); it != result.end(); ++it, ++n ) {
// 		it->print(); std::wcout << std::endl;
// 	    }

	    CPPUNIT_ASSERT( result.size() == 10 );
	    std::wstring resultWords[10] = { 
		L"teile", L"feile", L"teilen", L"teyle", L"theile", L"teller", L"feyle", L"teylen", L"theilen", L"theyle"
	    };
	    
	    n = 0;
	    for( DictSearch::CandidateSet::const_iterator it = result.begin(); it != result.end(); ++it, ++n ) {
//		it->print(); std::wcout << std::endl;
		CPPUNIT_ASSERT( it->getWord() == resultWords[ n ] );
	    }
	    
	    //////////////  ADD A HISTORIC DIC ////////////////////////
	    DictSearch::DictModule& histDict = ds.addDictModule( L"historic", std::string( "../csl/DictSearch/Test/small.historical.fbdic" ) );
	    histDict.setDLev( 0 );
	    result.clear();
	    ds.query( L"theile", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 10 );

	    histDict.setDLev( 1 );
	    result.clear();
	    ds.query( L"theile", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 12 );

	    ///////////// Add length-sensitive distance thresholds ///////////////
	    modernDict.setDLevWordlengths( 3, 7, 10 );


	}

	
	void testDLev() {
	    DictSearch ds;
	    DictSearch::DictModule& modernDict = ds.addDictModule( L"modern", std::string( "../csl/DictSearch/Test/small.modern.fbdic" ) );

	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 0 ) == 0 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 1 ) == 0 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 5 ) == 0 );

	    modernDict.setDLev( 1 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 0 ) == 1 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 1 ) == 1 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 5 ) == 1 );

	    modernDict.setDLevWordlengths( 3, 7, 10 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 0 ) == 0 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 2 ) == 0 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 3 ) == 1 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 4 ) == 1 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 7 ) == 2 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 8 ) == 2 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 10 ) == 3 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 12 ) == 3 );

	    // back to a static distance
	    modernDict.setDLev( 2 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 0 ) == 2 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 5 ) == 2 );
	    CPPUNIT_ASSERT( modernDict.getDLevByWordlength( 15 ) == 2 );

	}


	void testCaseMode() {
	    DictSearch ds;
	    

	    DictSearch::CandidateSet result;


	    DictSearch::DictModule& modernDict = ds.addDictModule( L"modern", std::string( "../csl/DictSearch/Test/small.modern.fbdic" ) );
	    modernDict.setCaseMode( Global::restoreCase );

	    ds.query( L"teile", &result );
	    CPPUNIT_ASSERT( result.size() == 1 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"teile"  );
	    CPPUNIT_ASSERT( result.at( 0 ).getInstruction().empty() );
	    
	    result.clear();
	    ds.query( L"Teile", &result );
	    CPPUNIT_ASSERT( result.size() == 1 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"Teile"  );
	    CPPUNIT_ASSERT( result.at( 0 ).getInstruction().empty() );
	    
	}

	void testStringDictModule() {


	    DictSearch ds;
	    

	    DictSearch::CandidateSet result;

	    StringDictModule histDict( ds, L"histDict", "./test.fbds" );

	    ds.addExternalDictModule( histDict );
	    //histDict.setCaseMode( Global::restoreCase );

	    ds.query( L"theil", &result );
	    
	    
	}
	
    private:
	
    };
    
} // namespace csl

#endif
