#include "csl/MinDic/MinDic.h"
#include "csl/Getopt/Getopt.h"
#include "csl/Global.h"
#include<iostream>


void printHelp(); // forward declaration

/**
 * @brief Program used to compute a MinDic<> from a word list and dump the automaton to a file.
 *
 */
int main(int argc, char const** argv) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt opt( argc, argv );

    if( opt.hasOption( "help" ) ) {
	    printHelp();
	    return EXIT_SUCCESS;
    }

    if( opt.hasOption( "stdin" ) ) {
	if( opt.getArgumentCount() != 1 ) {
	    printHelp();
	    return EXIT_FAILURE;
	}

	try {
	    csl::MinDic< int > t;

	    t.initConstruction();
	    
	    std::wstring line;
	    int annotation = 0;

	    while( std::getline( std::wcin, line ), std::wcin.good() ) {
		
		t.parseAnnotation( &line, &annotation );
		t.addToken( line.c_str(), annotation );
	    }
	    if( errno == EILSEQ ) { // catch encoding error
		throw csl::exceptions::badInput( "MinDic::compileDic: Encoding error in input sequence." );
	    }
	    t.finishConstruction();
	    
	    t.writeToFile( opt.getArgument( 0 ).c_str() );
	    
	    //   t.toDot();
	    //   t.printCells();
	    return EXIT_SUCCESS;
	    
	} catch ( csl::exceptions::cslException ex ) {
	    std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	    return EXIT_FAILURE;
	} catch ( std::exception ex ) {
	    std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	    return EXIT_FAILURE;
	}

    }
    else if( opt.getArgumentCount() == 2 ) {
	try {
	    csl::MinDic< int > t;
	    t.compileDic( argv[1] );
	    
	    t.writeToFile( argv[2] );
	    
	    //   t.toDot();
	    //   t.printCells();
	    
	} catch ( std::exception ex ) {
	    std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	    return EXIT_FAILURE;
	}
	
    }
    else {
	printHelp();
	return EXIT_FAILURE;
    }
    
}


void printHelp() {
    std::wcerr<<"Use like: compileMD <txtDic> <binDic>" <<std::endl;
}
