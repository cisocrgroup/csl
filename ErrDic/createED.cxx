#include<iostream>

#include "./ErrDic.h"
#include "./PatternApplier.h"
#include "../Getopt/Getopt.h"

int main( int argc, const char** argv ) {

    Getopt options( argc, argv );
    
    if( options.getArgumentCount() != 3 ) {
	std::cerr<<"Use like: "<<options.getProgName()<<" <pos._dic> <patternFile> <errDicFile>"<<std::endl;
	exit(1);
    }


    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */


    csl::MinDic< int > dic( options.getArgument( 0 ).c_str() );

//    dic.toDot(); // DEBUG


    csl::ErrDic ed;

    csl::PatternApplier pa( dic, options.getArgument( 1 ).c_str() );

    csl::MinDic< int > constraintDic;
    csl::MinDic< int > filterDic;

    if( options.hasOption( "constraintDic" ) ) {
	constraintDic.loadFromFile( options.getOption( "constraintDic" ).c_str() );
	pa.setConstraintDic( constraintDic );
    }

    if( options.hasOption( "filterDic" ) ) {
	filterDic.loadFromFile( options.getOption( "filterDic" ).c_str() );
	pa.setFilterDic( filterDic );
    }

    if( options.hasOption( "maxNrOfErrors" ) ) {
	pa.setMaxNrOfErrors( atoi( options.getOption( "maxNrOfErrors" ).c_str() ) );
    }

    pa.constructErrDic( ed );

    ed.writeToFile( options.getArgument( 2 ).c_str() );
}

