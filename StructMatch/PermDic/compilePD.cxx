#include<iostream>
#include "./PermDic.h"
#include "../../Alphabet/Alphabet.h"

using namespace csl;

int main(int argc, char** argv) {
    if(argc != 4) {
	std::cerr<<"Use like: "<<argv[0]<<" <alph> <txtDic> <binDic>"<<std::endl;
	exit(1);
    }
    
    Alphabet alph(argv[1]);
    PermDic pd(alph, argv[2]);
    pd.compileDic(argv[3]);
    

    return(0);
}
