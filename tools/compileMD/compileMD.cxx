#include "csl/MinDic/MinDic.h"
#include "csl/Getopt/Getopt.h"
#include "csl/Global.h"
#include<iostream>

/**
 * @brief Program used to compute a MinDic<> from a word list and dump the automaton to a file.
 *
 */
int main(int argc, char** argv) {
    //setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
    std::locale::global( std::locale( "") );
    if( argc != 3 ) {
	std::wcerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }



    try {
		csl::MinDic< int > t;
		t.compileDic( argv[1] );

		t.writeToFile( argv[2] );

	    //   t.toDot();
		//   t.printCells();

	} catch ( std::exception ex ) {
		std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
		return(1);
    }
    

}
