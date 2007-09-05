#include<iostream>
#include "../Alphabet/Alphabet.h"
#include "./MinDic.h"

using namespace csl;

int main( int argc, char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    if( argc < 2 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <binDic> [DOT|STAT]"<<std::endl;
	exit(1);
    }
    try {
	MinDic<> t;
	t.loadFromFile( argv[1] );
	if( argc == 3 && ! strcmp( argv[2], "DOT" ) ) {
	    t.toDot();
	}
	else if( argc == 3 && ! strcmp( argv[2], "STAT" ) ) {
	    t.doAnalysis();
	}
	else t.printDic();


	return 0;
    } catch ( exceptions::cslException ex ) {
	std::cerr << "Dictionary extraction failed: " << ex.what() << std::endl;
	return 1;
    }
}
