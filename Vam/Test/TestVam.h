#ifndef CSL_TESTMINDIC_H
#define CSL_TESTMINDIC_H CSL_TESTMINDIC_H

#include "../../Global.h"
#include "../MinDic.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVam : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVam );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestMinDic();

	void run();

	void testBasics();
// 	void testConstructionDetails();
// 	void lookupAllKeys();


    private:
	csl::Alphabet alph_;
	char* lexFile_;
	csl::MinDic<> mdic_;
	
    };

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestMinDic::testBasics() {


    }

//     void TestMinDic::lookupAllKeys() {
// 	std::ifstream fileHandle( lexFile_ );
// 	if ( !fileHandle.good() ) {
// 	    throw exceptions::badFileHandle( "Couldn't open file '" +
// 					     std::string( lexFile_ ) +
// 					     "' for reading." );
// 	}
// 	uchar line[Global::lengthOfLongStr];
// 	uchar* valueString = 0;
	
// 	bool testOk = true;
// 	size_t nrOfLines = 0;
// 	int value;
// 	while ( fileHandle.getline( ( char* ) line, Global::lengthOfLongStr ) )  {
// 	    uchar* c = ( uchar* )strchr( ( char* )line, Global::keyValueDelimiter );
	    
// 	    if( c ) {
// 		*c = 0;
// 		valueString = ( c + 1 );
// 	    }
// 	    uint_t pos = mdic_.walkStr( mdic_.getRoot(), line );
// 	    assert( mdic_.getAnnotation( line, &value ) );
// 	    testOk = testOk && pos && mdic_.isFinal( pos ) && ( atoi( (char*)valueString ) == value );
	    
// 	    ++nrOfLines;
// 	}
// 	assert( nrOfLines == mdic_.getNrOfKeys() );
// 	assert( testOk );
//     }
} // namespace csl


#endif
