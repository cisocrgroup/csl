#ifndef CSL_TEST_VAM_H
#define CSL_TEST_VAM_H CSL_TEST_VAM_H

#include "../../Global.h"
#include "../Vam.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVam : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVam );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:

	void testBasics();
// 	void testConstructionDetails();
// 	void lookupAllKeys();


    private:
	
    };

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestVam::testBasics() {
	CPPUNIT_ASSERT( 1 );
	CPPUNIT_ASSERT( 0 );

    }



} // namespace csl


#endif
