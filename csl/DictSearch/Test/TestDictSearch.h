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

	    // add a dict for exact matching
	    DictSearch ds;
	    ds.setModernDict( "dic1.mdic" );
	    CPPUNIT_ASSERT( false );
	    
	    
	    

	}
	
    private:
	
    };
    
} // namespace csl

#endif
