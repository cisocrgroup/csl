#include "./Trie.h"
#include<iostream>

using namespace csl;

int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    std::cout<<"Start compileTrie"<<std::endl;

    if(argc != 3) {
	std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    try {
	Trie t;
//    t.setByteAnnotation( true );
	t.compileDic( argv[1], argv[2] );

//	t.toDot();
//	t.printDic();
//    t.printCells();
    }
    catch( exceptions::cslException ex ) {
	std::cerr<<"compileTrie:"<<ex.what()<<std::endl;
	return 1;
    }

    return 0;
}
