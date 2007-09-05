#include "../Global.h"
#include "../Alphabet/Alphabet.h"
#include "./MinDic.h"
using namespace csl;

int main( size_t argc, char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    try {

	if(argc != 2) {
	    std::cerr<<"Use like: "<<argv[0]<<" <binDic>"<<std::endl;
	    exit(1);
	}

	MinDic< int > mdic;
	mdic.loadFromFile( argv[1] );

    
	uchar bytesIn[Global::lengthOfLongStr];
	wchar_t query[Global::lengthOfLongStr];
	// set the last byte to 0. So we can recognize when an overlong string was read by getline().
	bytesIn[Global::lengthOfLongStr - 1] = 0; 

	while( std::cin.getline( ( char* ) bytesIn, Global::lengthOfLongStr ) ) {
 	    if ( bytesIn[Global::lengthOfLongStr-1] != 0 ) {
		throw exceptions::badInput( "csl::compileMD: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    mbstowcs( query, (char*)bytesIn, Global::lengthOfLongStr );
	    
	    int ann = 0;
	    if( mdic.getAnnotation( query, &ann ) ) {
		std::cout<<ann<<std::endl;
	    }
	    else std::cout<<std::endl;
	}

    } catch( exceptions::cslException ex ) {
 	std::cout<<"lookupMD: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;
}
