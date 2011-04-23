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
	void testSTDIO();
	

    private:
	
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestCSLLocale );

    void TestCSLLocale::run() {
	testBasics();
	testSTDIO();
    }
    

    /**
     * test the basic methods
     */
    void TestCSLLocale::testBasics() {


	CPPUNIT_ASSERT_EQUAL_MESSAGE( "Test if decimal point is dot not comma", 
				      L'.', std::use_facet< std::numpunct< wchar_t > >( CSLLocale::Instance() ).decimal_point() );


	// read int
	int x_int = 0;
	CPPUNIT_ASSERT_NO_THROW( CSLLocale::string2number( L"42", &x_int ) );
	CPPUNIT_ASSERT_EQUAL( x_int, (int)42 );

	// throw exception for comma if not complete input can be interpreted as number
	CPPUNIT_ASSERT_THROW( CSLLocale::string2number( L"42bla", &x_int ), exceptions::cslException );

	// read float
	float x_float = 0;
	CPPUNIT_ASSERT_NO_THROW( CSLLocale::string2number( std::wstring( L"42.13" ), &x_float ) );
	CPPUNIT_ASSERT_EQUAL( x_float, (float)42.13 );

	// throw exception for comma as decimal point
	CPPUNIT_ASSERT_THROW( CSLLocale::string2number( L"42,13", &x_float ), exceptions::cslException );

    }

    void TestCSLLocale::testSTDIO() {
	std::wstring wideName;

	csl::string2wstring( std::wcout.getloc().name(), wideName );
	std::wcout << "Name " << wideName << std::endl;

	std::wcout.imbue( std::locale( "de_CH.utf8" ) );

	csl::string2wstring( std::wcout.getloc().name(), wideName );
	std::wcout << "Name " << wideName << std::endl;

	std::wcout.imbue( CSLLocale::Instance() );

	csl::string2wstring( std::wcout.getloc().name(), wideName );
	std::wcout << "Name " << wideName << std::endl;

	
    }

} // namespace csl


#endif
