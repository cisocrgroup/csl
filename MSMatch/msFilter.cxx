#include "./MSMatch.h"
#include "../ResultSet/ResultSet.h"
#include "../Alphabet/Alphabet.h"
#include "../Global.h"
using namespace csl;

uchar w[200];

int main( int argc, char** argv ) {
    if ( argc < 5 ) {
	std::cerr << "Use like: msFilter <alph> <dlev> <comp_dic> <rev_comp_dic>" << std::endl;
	exit( 1 );
    }

    Alphabet alph( argv[1] );

    try {
	MSMatch< FW_BW > matcher( alph, atoi( argv[2] ), argv[3], argv[4] );
//	MSMatch< STANDARD > matcher( alph, atoi( argv[2] ), argv[3], argv[4] );

	ResultSet list( alph );


	char query[200]; // = "and,artificiall,distributed,inteligence,machiene";

	while( 
//	    std::cout<<"$ "<<std::flush && 
	    std::cin >> query ) {

	    std::cout << "Query: " << query << std::endl;

	    list.reset(); // forget candidates that might be stored from earlier use
	    try {
		matcher.query( (uchar*)query, list );
	    } catch( exceptions::bufferOverflow exc ) {
		printf( "%s: %d\n",exc.what(), list.getSize() );
		exit(1);
	    }

//	    std::cout<<list.getSize()<<" hits."<<std::endl;
   	    list.sortUnique();
//  	    std::cout<<list.getSize()<<" hits."<<std::endl;


	    // print all hits
	    int i;
	    for( i = 0;i < list.getSize();++i ) {
		std::cout << list[i].getStr() <<","<<list[i].getAnn()<< std::endl;
	    }
	}
    }
    catch( exceptions::cslException exc ) {
	std::cerr << "msFilter caught exception: "<<exc.what() << std::endl;
    }

}
