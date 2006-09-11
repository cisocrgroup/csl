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

    
	char query[1000];
	while( 

//	std::cout<<"$ "<<std::flush && 
	    std::cin >> query ) {
	    
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
