#include "../Global.h"
#include "../Alphabet/Alphabet.h"
#include "./Cislex.h"
#include <string>
using namespace csl;
int main( size_t argc, char** argv ) {



    try {

	const Alphabet alph( argv[1] );
	Cislex cislex( alph );
	cislex.loadFromFile( argv[2] );

    
	char query[Global::lengthOfStr];
	query[Global::lengthOfStr - 1] = 0;

	while( 
//	std::cout<<"$ "<<std::flush && 
	    fgets( query, Global::lengthOfStr, stdin ) ) {

	    if( query[Global::lengthOfStr - 1] != 0 ) {
		throw exceptions::badInput( "lookupCislex: query string too long." );
	    }
	    query[strlen( ( char* )query )-1] = 0; // delete newline
	    const uchar* ret = cislex.lookup( (uchar*) query );
	    if( ret ) {
		std::cout<<ret<<std::endl;
	    }
	    else std::cout<<std::endl;
	}

    } catch( exceptions::cslException ex ) {
 	std::cout<<"lookupCislex: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;
}
