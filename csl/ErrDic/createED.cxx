#include<iostream>

#include "./ErrDic.h"
#include "./PatternApplier.h"
#include "../Getopt/Getopt.h"
#include "../Pattern/PatternSet.h"

int main( int argc, const char** argv ) {

    std::locale::global( std::locale( "" ) );

    Getopt options( argc, argv );
    
    if( options.getArgumentCount() != 3 ) {
	std::wcerr<<"createED"<<std::endl;
	std::wcerr<<"Use like: createED <baseLex.mdic> <patternFile> <errDicFile>"<<std::endl;
	std::wcerr<<"Creates all errors/variants that can possibly be derived from any word in <baseLex.mdic> by applying"<<std::endl;
	std::wcerr<<"transformation patterns as given in <patternFile>"<<std::endl;
	std::wcerr<<"Pattern file syntax: 1 pattern per line: <modern/correct> <SPACE> <old/erroneous>\n"<<std::endl;
	std::wcerr<<"As <errDicFile> you can use /dev/null if using the program for online construction of errors/variants"<<std::endl;
	std::wcerr<<"The tool is very experimental, so behaviour, output format etc. are subject to change considerably in later versions."<<std::endl;
	std::wcerr<<std::endl;
	std::wcerr<<"Options:"<<std::endl;
	std::wcerr<<"--constraintDic=<bla.mdic>      construct only errors/variants which are also found in bla.mdic"<<std::endl;
	std::wcerr<<"--filterDic=<blu.mdic>          Suppress errors/variants which also occur in blu.mdic"<<std::endl;
	std::wcerr<<"--minNrOfErrors=N               Suppress errors/variants with less than N applied patterns (default: 1)"<<std::endl;
	std::wcerr<<"--maxNrOfErrors=N               Suppress errors/variants with more than N applied patterns (default: 5000)"<<std::endl;
	std::wcerr<<std::endl;
	std::wcerr<<"Use compileMD to get appropriate mdic-automata files"<<std::endl;
	std::wcerr<<std::endl;
	std::wcerr<<"Uli Reffle, 2008 <uli@cis.uni-muenchen.de>"<<std::endl;
	
	exit(1);
    }


//    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
    std::locale::global( std::locale( "" ) );

    csl::MinDic< int > dic( options.getArgument( 0 ).c_str() );

//    dic.toDot(); // DEBUG


    csl::ErrDic ed;

//    csl::PatternSet patternSet();

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

    if( options.hasOption( "minNrOfErrors" ) ) {
	pa.setMinNrOfErrors( atoi( options.getOption( "minNrOfErrors" ).c_str() ) );
    }

    pa.constructErrDic( ed );

    ed.writeToFile( options.getArgument( 2 ).c_str() );
}

