#include "./MinDic.h"
#include<iostream>

using namespace csl;

#include "./MinDic.h"

int main(int argc, char** argv) {
    if(argc != 4) {
	std::cerr<<"Use like: "<<argv[0]<<" <alph> <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    Alphabet alph(argv[1]);
    MinDic t(alph);

    try {
	t.compileDic(argv[2],argv[3]);
    } catch ( exceptions::cslException ex ) {
	std::cout<<"compileMD failed: "<<ex.what()<<std::endl;
	return(1);
    }

//    t.toDot(&alph);
//    t.printCells();
//    t.printDic();

}
