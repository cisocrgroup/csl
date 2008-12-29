#include "csl/MinDic2/MinDic2.h"
#include "csl/Getopt/Getopt.h"
#include "csl/Global.h"
#include<iostream>

/**
 * @brief Program used to compute a csl::MinDic2 from a word list and dump the automaton to a file.
 *
 */
int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
    std::locale::global( std::locale( "") );
    
    if( argc != 3 ) {
	std::wcerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    csl::MinDic2 t;


    try {
	t.compileDic( argv[1] );
	
	t.writeToFile( argv[2] );
    } catch ( csl::exceptions::cslException ex ) {
	std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	return(1);
    }
    
    t.toDot();
    //t.printCells();
    //t.printDic();

}
