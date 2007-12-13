#include<iostream>

#include "./ErrDic.h"
#include "./PatternApplier.h"
#include "../Getopt/Getopt.h"

int main( int argc, char** argv ) {

    Getopt options( argc, argv );
    
    if( options.getArgumentCount() != 3 ) {
	std::cerr<<"Use like: "<<options.getProgName()<<" <pos._dic> <patternFile> <errDicFile>"<<std::endl;
	exit(1);
    }


    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */


    csl::MinDic< int > dic( argv[1] );

    


    csl::ErrDic ed;

    csl::PatternApplier pa( dic, argv[3] );

    csl::MinDic< int > constraintDic;
    csl::MinDic< int > filterDic;

    if( ! options.getOption( "constraintDic" ).empty() ) {
	filterDic.loadFromFile( options.getOption( "constraintDic" ).c_str() );
	pa.setConstraintDic( constraintDic );
    }

    if( ! options.getOption( "filterDic" ).empty() ) {
	filterDic.loadFromFile( options.getOption( "filterDic" ).c_str() );
	pa.setFilterDic( filterDic );
    }

    pa.constructErrDic( ed );

    ed.writeToFile( argv[4] );
}

