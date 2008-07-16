#ifndef CSL_TEST_VAAM_AS_ERRDIC_H
#define CSL_TEST_VAAM_AS_ERRDIC_H CSL_TEST_VAAM_AS_ERRDIC_H

#include "../../Global.h"
#include "../VaamAsErrDic.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {

    class TestVaamAsErrDic : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVaamAsErrDic );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVaamAsErrDic();

	void run();

	void testBasics();

    private:
	
    };
    CPPUNIT_TEST_SUITE_REGISTRATION( TestVaamAsErrDic );

    TestVaamAsErrDic::TestVaamAsErrDic() {
    }

    /**
     * test the basic methods
     */
    void TestVaamAsErrDic::testBasics() {

	MinDic<> baseDic( ( Global::cslRootDir + "csl/ErrDic/Test/small.base.mdic" ).c_str() );
	MinDic<> filterDic( ( Global::cslRootDir + "csl/ErrDic/Test/small.filter.mdic" ).c_str() );
	VaamAsErrDic vaamAsErrdic_( baseDic, ( Global::cslRootDir + "csl/ErrDic/Test/small.patterns.txt" ).c_str(), filterDic );

	Interpretation answer;

	// a classical error
	CPPUNIT_ASSERT( vaamAsErrdic_.query( L"xachen", &answer ) );

	// a false friend filtered by the filterDic
	CPPUNIT_ASSERT( ! vaamAsErrdic_.query( L"hanne", &answer ) );
    }

} // namespace csl


#endif
