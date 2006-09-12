#include "../Alphabet/Alphabet.h"
#include "./Trie.h"
#include<iostream>

using namespace csl;

int main(int argc, char** argv) {
    if(argc != 4) {
	std::cerr<<"Use like: "<<argv[0]<<" <alph> <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    try {
    Alphabet alph( argv[1] );
    Trie t( alph );
//    t.setByteAnnotation( true );
    t.compileDic( argv[2], argv[3] );
//    t.toDot( &alph );
    }
    catch( exceptions::cslException ex ) {
	std::cerr<<"csl::compileTrie: Caught exception: "<<ex.what()<<std::endl;
	return 1;
    }

    return 0;
}
