#ifndef CSL_TESTERRDIC_H
#define CSL_TESTERRDIC_H CSL_TESTERRDIC_H

#include "../../Global.h"
#include "../ErrDic.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestErrDic : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestErrDic );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestErrDic();

	void run();

	void testBasics();
// 	void testConstructionDetails();
// 	void lookupAllKeys();


    private:
	csl::ErrDic errdic_;
	
    };

    /**
     * test the basic methods
     */
    void TestErrDic::testBasics() {
    }


} // namespace csl


#endif
