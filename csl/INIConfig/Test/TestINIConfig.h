#ifndef OCRC_TESTINICONFIG_H
#define OCRC_TESTINICONFIG_H OCRC_TESTINICONFIG_H

#include<fstream>
#include<string.h>

#include "../INIConfig.h"

#include<cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestINIConfig : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestINIConfig );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testLongLine );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestINIConfig();

	void run();

	void testBasics();
	void testLongLine();


    private:
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestINIConfig );

    TestINIConfig::TestINIConfig() {

    }

    void TestINIConfig::run() {
	testBasics();
	testLongLine();
    }
    

    /**
     * test the basic methods
     */
    void TestINIConfig::testBasics() {
	std::ofstream iniFile( "test.ini" );
	iniFile << "int_test = 42" << std::endl;
	iniFile << "double_test = 42.4242" << std::endl;
	iniFile << "string_test = 'passt scho'" << std::endl;
	iniFile << "[eineKategorie]" << std::endl;
	iniFile << "string_test = 'passt immer no'" << std::endl;
	iniFile.close();

	INIConfig iniconf( "test.ini" );

	CPPUNIT_ASSERT( iniconf.getint( ":int_test" ) == 42 );
	CPPUNIT_ASSERT( iniconf.getdouble( ":double_test" ) == 42.4242 );
	CPPUNIT_ASSERT( ! strcmp( iniconf.getstring( ":string_test" ), "passt scho" ) );
	CPPUNIT_ASSERT( ! strcmp( iniconf.getstring( "eineKategorie:string_test" ), "passt immer no" ) );
    }

    void TestINIConfig::testLongLine() {
	std::ofstream iniFile( "test.ini" );
	iniFile << "A very long line, longer than 1024:";
	for( size_t i = 0 ; i < 1050; ++i ) {
	    iniFile << "x";
	}
	iniFile << std::endl;
	iniFile.close();
	
	CPPUNIT_ASSERT_THROW( INIConfig iniconf( "test.ini" ), exceptions::cslException  );
	
    }

} // eon
#endif
