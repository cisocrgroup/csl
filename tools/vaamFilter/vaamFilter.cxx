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


#ifdef CSL_VAAMFILTER_USE_TRANSTABLE
typedef csl::Vaam< csl::TransTable< csl::TT_PERFHASH, uint16_t, uint32_t > > Vaam_t;
#else
typedef csl::Vaam< csl::MinDic<> > Vaam_t;
#endif


int main(int argc, const char** argv ) {

	try {

    std::locale::global( std::locale("") ); // set the environment's default locale
    Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 3 ) {
	std::wcerr<< "Use like: vaamFilter [options] <distance> <dictionary> <pattern-file>"<<std::endl
		  << std::endl
		  << "Options:" << std::endl
		  << "--minNrOfPatterns=N       Allow only interpretations with N or more pattern applications. Defaults to 0." << std::endl
		  << "--maxNrOfPatterns=N       Allow only interpretations with at most N pattern applications. Defaults to INFINITE." << std::endl
		  << "--machineReadable=1       Print (even more) machine-readable output, i.e. all answers in one line, separated by '|'" << std::endl;
	exit( 1 );
    }

    Vaam_t::MinDic_t baseDic;
    baseDic.loadFromFile( opt.getArgument( 1 ).c_str() );

    Vaam_t vaam( baseDic, opt.getArgument( 2 ).c_str() );

    if( opt.hasOption( "maxNrOfPatterns" ) ) {
	vaam.setMaxNrOfPatterns( atoi( opt.getOption( "maxNrOfPatterns" ).c_str() ) );
    }

    if( opt.hasOption( "minNrOfPatterns" ) ) {
	vaam.setMinNrOfPatterns( atoi( opt.getOption( "minNrOfPatterns" ).c_str() ) );
    }

    bool machineReadable = false;
    if( opt.hasOption( "machineReadable" ) ) {
	machineReadable = true;
    }


    Vaam_t::CandidateReceiver answers;

    std::wstring query;

    size_t maxDistance = atoi( opt.getArgument( 0 ).c_str() );
    vaam.setDistance( maxDistance );
    Stopwatch watch;

    if( machineReadable ) {
	std::wcout << "csl::Vaam: READY [machineReadable=true]" << std::endl;
    }
    else {
	std::wcout << "csl::Vaam: READY [machineReadable=false]" << std::endl;
    }

    while( std::wcin >> query ) {
	watch.start();
	answers.clear();
	vaam.query( query, &answers );

	std::sort( answers.begin(), answers.end() );

	if( answers.empty() ) {
#ifndef CSL_VAAMFILTER_PRINTNONE
	    std::wcout<<query<<":NONE"<<std::endl;
#endif
	}
	else if( machineReadable ) {
	    // all interpretations of the query in one line
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
