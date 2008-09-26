#ifndef CSL_TEST_DICTSEARCH_H
#define CSL_TEST_DICTSEARCH_H CSL_TEST_DICTSEARCH_H

#include <cppunit/extensions/HelperMacros.h>

#include"../DictSearch.h"

namespace csl {
    class TestDictSearch : public CppUnit::TestFixture {
    public:

	CPPUNIT_TEST_SUITE( TestDictSearch );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();

	
	
	void testBasics() {

	    // add a modern dict for exact matching, dlev==0
	    DictSearch ds;
	    ds.setModernDict( "../csl/DictSearch/Test/dic1.fbdic", 0 );
	    
	    
	    DictSearch::CandidateSet result;
	    ds.query( L"komma", result );

	    for( DictSearch::CandidateSet::const_iterator it = result.begin(); it != result.end(); ++it ) {
		it->print();
	    } 
	    CPPUNIT_ASSERT( result.size() == 1 );
	    CPPUNIT_ASSERT( result.at( 0 ).getWord() == L"komma"  );
	    CPPUNIT_ASSERT( true );
	    result.at( 0 ).getInstruction().empty();
	}
	
    private:
	
    };
    
} // namespace csl

#endif
