#include <csl/ErrDic/ErrDic.h>
#include<iostream>

using namespace csl;

int main(int argc, char** argv) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    try {

	if(argc != 2) {
	    std::cerr<<"Use like: "<<argv[0]<<" <binDic>"<<std::endl;
	    exit(1);
	}

	ErrDic t;
	t.loadFromFile( argv[1] );

//	t.doAnalysis(); exit( 0 );

	ErrDic::Entry e;

	std::wstring query;
	while( std::getline( std::wcin, query ).good() ) {
	    if ( query.length() > Global::lengthOfLongStr ) {
		throw exceptions::badInput( "csl::lookupED: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }

	    if( t.lookup( query.c_str(), &e ) ) {
		printf( "original='%ls', pattern='%ls'\n", e.getOriginal(), e.getErrorPattern() );
	    }
	    else printf( "\n" );

	}
	if( errno == EILSEQ ) {
	    throw exceptions::badInput( "csl::lookupED: Input encodig error" );
	}

    } catch( exceptions::cslException ex ) {
 	std::cout<<"lookupED: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;

}
