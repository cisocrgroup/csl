#ifndef CSL_TESTCSLLOCALE_H
#define CSL_TESTCSLLOCALE_H CSL_TESTCSLLOCALE_H


#include <cppunit/extensions/HelperMacros.h>

#include "../CSLLocale.h"

namespace csl {
    class TestCSLLocale : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestCSLLocale );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:

	void run();

	void testBasics();
	

    private:
	
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestCSLLocale );

    void TestCSLLocale::run() {
	testBasics();
    }
    

    /**
     * test the basic methods
     */
    void TestCSLLocale::testBasics() {
	CSLLocale& loc = CSLLocale::Instance();
    }


} // namespace csl


#endif
