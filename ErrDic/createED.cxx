#include<iostream>

#include "./ErrDic.h"
#include "./ErrDicConstructor.h"

int main( int argc, char** argv ) {

    if( argc != 5 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <pos._dic> <filterDic> <patternFile> <errDicFile>"<<std::endl;
	exit(1);
    }


    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    csl::MinDic< int > dic( argv[1] );
    csl::MinDic< int > filterDic( argv[2]);


    csl::ErrDic ed;

    csl::ErrDicConstructor edc;

    edc.constructErrDic( dic, filterDic, argv[3], ed );

    ed.writeToFile( argv[4] );
}

