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

	void testBasics();
	void testConstructionDetails();
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
	testBasics();
	testConstructionDetails();
	lookupAllKeys();
    }
    
    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestMinDic::testBasics() {
	char key[20] = "aachen";
	/*
	 * walk key char by char and test if
	 * - key could be walked completely
	 * - pos_1 is a valid state of the automaton
	 * - pos_1 is a final state
	 */
	uint_t pos_1 = mdic_.getRoot(); // set pos_1 to root
	char* c = 0;
	for( c = key; *c && pos_1; ++c ) {
	    pos_1 = mdic_.walk( pos_1, alph_.code( *c ) );
	}
	assert( ! *c );
	assert( pos_1 );
	assert( mdic_.isFinal( pos_1 ) );
	int ann = 0;

	/*
	 * test if the same result is returned by walkStr
	 */
	uint_t pos_2 = mdic_.walkStr( mdic_.getRoot(), (uchar*)key );
	assert( pos_1 == pos_2 );

	/*
	 * test if walking with the empty string works
	 */
	assert( mdic_.walkStr( mdic_.getRoot(), (uchar*)key ) == mdic_.getRoot() );

	/*
	 * test if the proper annotation is returned for the key
	 */
	assert( mdic_.getAnnotation( (uchar*)key, &ann ) && ( ann == 15 ) );

	/*
	 * Test if walking with an invalid char is just failing and not causing any trouble
	 */
	pos_1 = mdic_.getRoot(); // set pos_1 to root
	assert( alph_.code( '#' ) == 0 );
	assert( mdic_.walk( pos_1, alph_.code( '#' ) ) == 0 );

	assert( ! mdic_.walkStr( mdic_.getRoot(), (uchar*)"dies#das" ) );
	assert( ! mdic_.walkStr( mdic_.getRoot(), (uchar*)"dies#" ) );


    }

    void TestMinDic::testConstructionDetails() {
	MinDic mdic( alph_ );
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
