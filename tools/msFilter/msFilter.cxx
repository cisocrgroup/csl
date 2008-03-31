#include "csl/MSMatch/MSMatch.h"
#include "csl/ResultSet/ResultSet.h"
#include "csl/Global.h"
using namespace csl;


int main( int argc, char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    if ( argc != 3 ) {
	std::cerr << "Use like: msFilter <dlev> <fb_dic>" << std::endl;
	exit( 1 );
    }

    try {
	MSMatch< FW_BW > matcher( atoi( argv[1] ), argv[2] );
//	MSMatch< STANDARD > matcher( atoi( argv[1] ), argv[2] );

	ResultSet list;


	uchar bytesIn[Global::lengthOfLongStr]; // = "and,artificiall,distributed,inteligence,machiene";
	wchar_t query[Global::lengthOfLongStr];

	// set the last byte to 0. So we can recognize when an overlong string was read by getline().
	bytesIn[Global::lengthOfLongStr - 1] = 0; 

	while( std::cin.getline( ( char* ) bytesIn, Global::lengthOfLongStr ) ) {
 	    if ( bytesIn[Global::lengthOfLongStr-1] != 0 ) {
		throw exceptions::badInput( "csl::msFilter: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    mbstowcs( query, (char*)bytesIn, Global::lengthOfLongStr );

	    printf( "Query: %ls\n", query );

	    list.reset(); // forget candidates that might be stored from earlier use

	    try {
		matcher.query( query, list );
	    } catch( exceptions::bufferOverflow exc ) {
		fprintf( stderr, "%s: %d\n",exc.what(), list.getSize() );
	    }

// 	    std::cout<<list.getSize()<<" hits."<<std::endl;
//    	    list.sortUnique();
//   	    std::cout<<list.getSize()<<" hits."<<std::endl;


	    // print all hits
	    size_t i;
	    for( i = 0;i < list.getSize();++i ) {
		printf( "%ls,%d,%d\n", list[i].getStr(), list[i].getLevDistance(), list[i].getAnn() );
	    }
	}
    }
    catch( exceptions::cslException exc ) {
	std::cerr << "msFilter caught exception: "<< exc.what() << std::endl;
    }

}
