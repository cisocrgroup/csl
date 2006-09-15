#ifndef TESTMINDIC_H
#define TESTMINDIC_H TESTMINDIC_H

#include "../../Global.h"
#include "../../Test/Test.h"
#include "../MinDic.h"


namespace csl {
    class TestMinDic : public Test  {
    public:
	TestMinDic();

	void run();

	void lookupAllKeys();

    private:
	csl::Alphabet alph_;
	char* lexFile_;
	csl::MinDic mdic_;
	
    };

    TestMinDic::TestMinDic() : Test( "csl::MinDic" ),
			       alph_( "./MinDic/Test/test.azb" ),
			       lexFile_( "./MinDic/Test/test.lex" ),
			       mdic_( alph_ ) {
	try {
	    mdic_.compileDic( "./MinDic/Test/test.lex" );
	} catch ( exceptions::cslException exc ) {
	    std::cout << exc.what() << std::endl;
	    return;
	}
    }

    void TestMinDic::run() {
	lookupAllKeys();
    }

    void TestMinDic::lookupAllKeys() {
	std::ifstream fileHandle( lexFile_ );
	if ( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" +
					     std::string( lexFile_ ) +
					     "' for reading." );
	}
	uchar line[Global::lengthOfLongStr];
	uchar* valueString = 0;
	
	bool testOk = true;
	size_t nrOfLines = 0;
	int value;
	while ( fileHandle.getline( ( char* ) line, Global::lengthOfLongStr ) )  {
	    uchar* c = ( uchar* )strchr( ( char* )line, Global::keyValueDelimiter );
	    
	    if( c ) {
		*c = 0;
		valueString = ( c + 1 );
	    } 
	    uint_t pos = mdic_.walkStr( mdic_.getRoot(), line );
	    assert( mdic_.getAnnotation( line, &value ) );
	    testOk = testOk && pos && mdic_.isFinal( pos ) && ( atoi( (char*)valueString ) == value );
	    
	    ++nrOfLines;
	}
	assert( nrOfLines == mdic_.getNrOfKeys() );
	assert( testOk );
    }

} // namespace csl


#endif
