#include<iostream>
#include "csl/Vam/Vam.h"
#include "csl/Getopt/Getopt.h"
#include "csl/Stopwatch.h"

/**
 * Vaam
 * 
 * @file vaamFilter
 * 
 * @brief vaamFilter is a command-line tool for the usage of the class Vaam.
 * It is invoked with a distance bound \c k, a compiled minimized dictionary \c dic
 * and a file containing a set of patterns \c P.
 * 
 * 
 * Please consult the documentation of class csl::Vaam for details.
 *
 * @see csl::Vaam
 * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
 * 
 */
int main(int argc, const char** argv ) {

    std::locale::global( std::locale( "de_DE.utf-8" ) );

    Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 3 ) {
	std::wcerr<<"Use like: vamFilter <distance> <dictionary> <pattern-file>"<<std::endl;
	exit( 1 );
    }

    csl::MinDic<> baseDic;
    baseDic.loadFromFile( opt.getArgument( 1 ).c_str() );

    csl::Vam vam( baseDic, opt.getArgument( 2 ).c_str() );

    std::vector< csl::Vam::Answer > answers;

    std::wstring query;

    size_t maxDistance = atoi( opt.getArgument( 0 ).c_str() );
    vam.setDistance( maxDistance );
    Stopwatch watch;

    while( std::wcin >> query ) {
	watch.start();
	answers.clear();
	vam.query( query, &answers );

	if( answers.empty() ) {
	    std::wcout<<query<<":NONE"<<std::endl;
	}
	else if( maxDistance == 0 ) {
	    // all interpretations of the query in one line
	    if( maxDistance != 0 ) throw csl::exceptions::cslException( "This kind of output is suitable only for output 0" );
	    
	    for( std::vector< csl::Vam::Answer >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		if( it + 1  != answers.end() ) std::wcout<<"|";
	    }
	    std::wcout<<std::endl;
	}
	else {
	    // new line for each interpretation of the query
	    for( std::vector< csl::Vam::Answer >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		std::wcout<<std::endl;
	    }
	}

	
//	std::wcout<<watch.readMilliseconds()<<" ms"<<std::endl;
    }

    
    return 0;
}
