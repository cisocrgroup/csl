#include <csl/ErrDic/ErrDic.h>
#include<iostream>

using namespace csl;

int main(int argc, char** argv) {
	std::locale::global( CSL_UTF8_LOCALE );
 	std::wcerr.imbue( CSL_UTF8_LOCALE );
 	std::wcout.imbue( CSL_UTF8_LOCALE );

    if( argc != 3 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    ErrDic t;

    try {
	t.compileDic( argv[1] );
	t.writeToFile( argv[2] );
    } catch ( exceptions::cslException ex ) {
	std::cout<<"compileED failed: "<<ex.what()<<std::endl;
	return(1);
    }

//    t.toDot();
//    t.printCells();
//    t.printDic();

    return 0;

	uchar bytesIn[Global::lengthOfLongStr];
	wchar_t query[Global::lengthOfLongStr];
	// set the last byte to 0. So we can recognize when an overlong string was read by getline().
	bytesIn[Global::lengthOfLongStr - 1] = 0; 

	ErrDic::Entry e;
	while( std::cin.getline( ( char* ) bytesIn, Global::lengthOfLongStr ) ) {
 	    if ( bytesIn[Global::lengthOfLongStr-1] != 0 ) {
		throw exceptions::badInput( "csl::MinDic::compileDic:Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    mbstowcs( query, (char*)bytesIn, Global::lengthOfLongStr );

	    if( t.lookup( query, &e ) ) {
		printf( "ha: original='%ls', pattern='%ls'\n", e.getOriginal(), e.getErrorPattern() );
	    }
	    else printf( "\n" );

	}

}
