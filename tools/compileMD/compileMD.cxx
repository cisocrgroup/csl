#include "csl/csl.h"
#include<iostream>


int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
    
    if( argc != 3 ) {
	std::wcerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

	csl::MinDic< int > t;


    try {
		t.compileDic( argv[1] );

		t.writeToFile( argv[2] );
	} catch ( csl::exceptions::cslException ex ) {
		std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
		return(1);
    }

    // t.toDot();
 //   t.printCells();
 //   t.printDic();

}
