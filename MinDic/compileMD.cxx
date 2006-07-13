#include "./MinDic.h"
#include<iostream>

using namespace csl;

int main(int argc, char** argv) {
    if(argc != 4) {
	std::cerr<<"Use like: "<<argv[0]<<" <alph> <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    Alphabet alph(argv[1]);
    MinDic t(alph);
    t.compileDic(argv[2],argv[3]);

//    t.toDot(&alph);
//    t.printCells();
//    t.printDic();

}
