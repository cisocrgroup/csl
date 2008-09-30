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
	    ds.setModern( "../csl/DictSearch/Test/dic1.fbdic", 0 );
	    ds.query( L"komma", result );
	    CPPUNIT_ASSERT( result.size() == 1 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"komma"  );
	    CPPUNIT_ASSERT( result.at( 0 ).getInstruction().empty() );

	    // change dlev to 1, 2
	    result.clear();
	    ds.setModernDlev( 1 );
	    ds.query( L"komma", result );
	    CPPUNIT_ASSERT( result.size() == 2 );

	    ds.setModernDlev( 2 );
	    result.clear();
	    ds.query( L"komma", result );
	    CPPUNIT_ASSERT( result.size() == 3 );

	    // init the hpothetical dic, with dlev 0
	    ds.initHypothetic( "../csl/DictSearch/Test/small.patterns.txt", 0 );
	    result.clear();
	    ds.query( L"komma", result );
	    CPPUNIT_ASSERT( result.size() == 3 );

	    for( DictSearch::CandidateSet::const_iterator it = result.begin(); it != result.end(); ++it ) {
		it->print();
	    }
	    
	    

	}
	
    private:
	
    };
    
} // namespace csl

#endif
