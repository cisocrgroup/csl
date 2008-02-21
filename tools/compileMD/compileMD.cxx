#include "../../csl/csl.h"
#include<iostream>


int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
    
    if( argc != 3 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

	std::wcout<<"TEST1"<<std::endl;

	csl::MinDic< int > t;

	std::wcout<<"TEST2"<<std::endl;

    try {
		std::wcout<<"TEST3"<<std::endl;
		t.compileDic( argv[1] );
		std::wcout<<"TEST4"<<std::endl;

	//	t.writeToFile( argv[2] );
	} catch ( csl::exceptions::cslException ex ) {
		std::cout<<"compileMD failed: "<<ex.what()<<std::endl;
		return(1);
    }

 //   t.toDot();
 //   t.printCells();
 //   t.printDic();

}
