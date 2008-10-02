#ifndef CSL_TEST_DICTSEARCH_H
#define CSL_TEST_DICTSEARCH_H CSL_TEST_DICTSEARCH_H

#include <cppunit/extensions/HelperMacros.h>

#include"../DictSearch.h"

namespace csl {
    class TestDictSearch : public CppUnit::TestFixture {
    public:

	CPPUNIT_TEST_SUITE( TestDictSearch );
	CPPUNIT_TEST( smokeTest );
	CPPUNIT_TEST_SUITE_END();

	
	
	void smokeTest() {

	    // add a modern dict for exact matching, dlev==0
	    DictSearch ds;

	    
	    
	    DictSearch::CandidateSet result;

	    // lookup in modern dict with dlev 0
	    ds.setModern( "../csl/DictSearch/Test/small.modern.fbdic", 0 );
	    ds.query( L"teile", &result );
	    CPPUNIT_ASSERT( result.size() == 1 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"teile"  );
	    CPPUNIT_ASSERT( result.at( 0 ).getInstruction().empty() );

	    // change dlev to 1, 2
	    result.clear();
	    ds.setModernDlev( 1 );
	    ds.query( L"teile", &result );
	    CPPUNIT_ASSERT( result.size() == 2 );

	    ds.setModernDlev( 2 );
	    result.clear();
	    ds.query( L"teile", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 3 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"teile"  );
	    CPPUNIT_ASSERT( result.at( 1 ).getWord() == L"teilen"  );
	    CPPUNIT_ASSERT( result.at( 2 ).getWord() == L"teller"  );

	    // init the hpothetic dic, with dlev 0
	    ds.initHypothetic( "../csl/DictSearch/Test/small.patterns.txt", 0 );
	    result.clear();
	    ds.query( L"theyle", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 2 );

	    // set dlev=2 for hpothetic dic
	    ds.setHypotheticDlev( 2 );
	    result.clear();
	    ds.query( L"teile", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 8 );
	    std::wstring resultWords[8] = { 
		L"teile", L"teilen", L"teyle", L"theile", L"teller", L"teylen", L"theilen", L"theyle"
	    };
	    
	    size_t n = 0;
	    for( DictSearch::CandidateSet::const_iterator it = result.begin(); it != result.end(); ++it, ++n ) {
//		it->print(); std::wcout << std::endl;
		CPPUNIT_ASSERT( it->getWord() == resultWords[ n ] );
	    }
	    
	    //////////////  ADD A HISTORIC DIC ////////////////////////
	    ds.setHistoric( "../csl/DictSearch/Test/small.historic.fbdic", 0 );
	    result.clear();
	    ds.query( L"theile", &result );
	    std::sort( result.begin(), result.end() );
	    CPPUNIT_ASSERT( result.size() == 9 );


	    CPPUNIT_ASSERT( false );

	}
	
    private:
	
    };
    
} // namespace csl

#endif
