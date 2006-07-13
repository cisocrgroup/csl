#include<iostream>

#include "../Alphabet/Alphabet.h"
#include "./Trie.h"

using namespace csl;

int main(int argc, char** argv) {
    try {
	Alphabet alph(argv[1]);
	Trie t(alph,argv[2]);
	
	t.printDic(0);
	return 0;
    }
    catch( exceptions::cslException ex ) {
	std::cerr<<std::endl<<"Dictionary extraction failed: "<<ex.what()<<std::endl;
	return 1;
    }
}
