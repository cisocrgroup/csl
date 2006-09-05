#include<iostream>
#include "../Alphabet/Alphabet.h"
#include "./MinDic.h"

using namespace csl;

int main( int argc, char** argv ) {
    try {
	Alphabet alph( argv[1] );
	MinDic t( alph );
	t.loadFromFile( argv[2] );
	t.printDic();

	return 0;
    } catch ( exceptions::cslException ex ) {
	std::cerr << "Dictionary extraction failed: " << ex.what() << std::endl;
    return 1;
  }
}
