#include <csl/FBDic/FBDic.h>
#include<iostream>

using namespace csl;

int main(int argc, char** argv) {

    std::locale::global( std::locale("") );
    if( argc != 3 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    FBDic< int > t;

    try {
	t.compileDic( argv[1] );
	t.writeToFile( argv[2] );
    } catch ( exceptions::cslException ex ) {
	std::cout<<"compileFBDic failed: "<<ex.what()<<std::endl;
	return(1);
    }

//    t.toDot();
//    t.printCells();
//    t.printDic();

}
