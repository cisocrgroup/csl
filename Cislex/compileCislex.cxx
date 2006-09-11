#include "../Global.h"
#include "../Alphabet/Alphabet.h"
#include "./Cislex.h"

using namespace csl;
int main( size_t argc, char** argv ) {

    const Alphabet alph( argv[1] );

    Cislex cislex( alph );
    
    cislex.compileDic( argv[2] );
    cislex.writeToFile( argv[3] );

    char query[200]; // = "and,artificiall,distributed,inteligence,machiene";

    while( 
	std::cout<<"$ "<<std::flush && 
	std::cin >> query ) {
	
	const uchar* ret = cislex.lookup( (uchar*) query );
	if( ret ) {
	    std::cout<<cislex.lookup( (uchar*) query )<<std::endl;
	}
	else {
	    std::cout<<"Key not found"<<std::endl;
	}
    }

    return 0;
}
