#include<iostream>
#include "csl/Vaam/Vaam.h"
#include "csl/Getopt/Getopt.h"
#include "csl/Stopwatch.h"

//#define CSL_VAAMFILTER_PRINTNONE true

/**
 * Vaam
 * 
 * @file
 * @brief vaamFilter is a command-line tool for the usage of the class Vaam.
 * It is invoked with a distance bound \c k, a compiled minimized dictionary \c dic
 * and a file containing a set of patterns \c P.
 * 
 * Please consult the documentation of class csl::Vaam for details.
 *
 * @see csl::Vaam
 * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
 * 
 */
int main(int argc, const char** argv ) {

	try {

    std::locale::global( CSL_UTF8_LOCALE );
    Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 3 ) {
	std::wcerr<<"Use like: vaamFilter <distance> <dictionary> <pattern-file>"<<std::endl;
	exit( 1 );
    }

    csl::MinDic<> baseDic;
    baseDic.loadFromFile( opt.getArgument( 1 ).c_str() );

    csl::Vaam<> vaam( baseDic, opt.getArgument( 2 ).c_str() );

    if( opt.hasOption( "maxNrOfPatterns" ) ) {
	vaam.setMaxNrOfPatterns( atoi( opt.getOption( "maxNrOfPatterns" ).c_str() ) );
    }


    csl::Vaam<>::CandidateReceiver answers;

    std::wstring query;

    size_t maxDistance = atoi( opt.getArgument( 0 ).c_str() );
    vaam.setDistance( maxDistance );
    Stopwatch watch;

    while( std::wcin >> query ) {
	watch.start();
	answers.clear();
	vaam.query( query, &answers );

	if( answers.empty() ) {
#ifndef CSL_VAAMFILTER_PRINTNONE
	    std::wcout<<query<<":NONE"<<std::endl;
#endif
	}
	else if( maxDistance == 0 ) {
	    // all interpretations of the query in one line
	    if( maxDistance != 0 ) throw csl::exceptions::cslException( "This kind of output is suitable only for output 0" );
	    
#ifndef CSL_VAAMFILTER_PRINTNONE
	    for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		if( it + 1  != answers.end() ) std::wcout<<"|";
	    }
	    std::wcout<<std::endl;
#endif
	}
	else {
	    // new line for each interpretation of the query
#ifndef CSL_VAAMFILTER_PRINTNONE
	    for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		std::wcout<<std::endl;
	    }
#endif
	}

	
//	std::wcout<<watch.readMilliseconds()<<" ms"<<std::endl;
    }
	
	} catch( csl::exceptions::cslException ex ) {
		std::wcout<<"Caught exception: "<<ex.what()<< std::endl;
		return 0;
	}
		    
    return 0;
}
