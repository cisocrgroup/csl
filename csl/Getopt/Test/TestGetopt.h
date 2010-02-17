#ifndef CSL_TESTGETOPT_H
#define CSL_TESTGETOPT_H CSL_TESTGETOPT_H

#include "../../Global.h"
#include "../Getopt.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestGetopt : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestGetopt );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:

	void run();

	void testBasics();

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestGetopt );

//     TestGetopt::TestGetopt() {
//     }

    void TestGetopt::run() {
	testBasics();
    }
    

    /**
     * test the basic functionality
     */
    void TestGetopt::testBasics() {

	// test old way, without any specification of options
	char const* argv[] = { "progname", "--flag1=1", "--flag2=value", "arg1", "arg2" };
	int argc = 5;
	
	Getopt getopt;
	getopt.getOptions( argc, argv );

	CPPUNIT_ASSERT( getopt.hasOption( "flag1" ) );
	CPPUNIT_ASSERT( getopt.hasOption( "flag2" ) );

	CPPUNIT_ASSERT( getopt.getOption( "flag1" ) == "1" );
	CPPUNIT_ASSERT( getopt.getOption( "flag2" ) == "value" );

	CPPUNIT_ASSERT( getopt.getArgumentCount() == 2 );
	CPPUNIT_ASSERT( getopt.getArgument( 0 ) == "arg1" );
	CPPUNIT_ASSERT( getopt.getArgument( 1 ) == "arg2" );
    }

} // namespace csl


#endif
