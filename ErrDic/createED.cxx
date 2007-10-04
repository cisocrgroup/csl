#include<iostream>

#include "./ErrDic.h"
#include "./PatternApplier.h"

int main( int argc, char** argv ) {

    if( argc != 4 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <pos._dic> <filterDic> <patternFile>"<<std::endl;
	exit(1);
    }


    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    csl::MinDic< int > dic( argv[1] );
    csl::MinDic< int > filterDic( argv[2]);


    csl::ErrDic ed;
    csl::PatternApplier applier( dic, filterDic, argv[3] );

    while( applier.isGood() ) {
	std::wcout<<applier.getWord()<<std::endl;
    }

    
    
}

