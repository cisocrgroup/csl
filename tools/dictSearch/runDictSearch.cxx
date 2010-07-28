#include <locale>
#include <csl/DictSearch/DictSearch.h>
#include <csl/Getopt/Getopt.h>

// uncomment this if you want so suppress output for the words
//#define CSL_DICTSEARCH_PRINTNONE 1;

int main( int argc, char const** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    csl::Getopt options( argc, argv );

    if( options.hasOption( "help" ) || !options.hasOption( "patternFile" ) ) {
	std::wcerr<< std::endl
		  << "Use like: runDictSearch"<<std::endl
		  << "--modernDict=<dictFile>       Modern vocabulary (obligatory)" << std::endl
		  << "--patternFile=<patternFile>   Patternset to use (obligatory)"<< std::endl
		  << "--histDict=<dictFile>         Vocabulary from a historical corpus" << std::endl
		  << "--NEDict=<dictFile>           Named-entities" << std::endl
		  << "--machineReadable=1           Print (even more) machine-readable output, i.e. all answers in one line, separated by '|'" << std::endl
		  << std::endl<<std::endl;
	exit(1);
    }
    
    // create a DictSearch-object
    csl::DictSearch dictSearch;

    // initialise the hypothetic dict. with a file of patterns
    dictSearch.initHypothetic( options.getOption( "patternFile" ).c_str() );

    // set a modern dictionary
    if( options.hasOption( "modernDict" ) ) {
	csl::DictSearch::DictModule& modernMod = dictSearch.addDictModule( L"modern", options.getOption( "modernDict" ) );
	modernMod.setDLevWordlengths();
	modernMod.setPriority( 100 );
	modernMod.setMaxNrOfPatterns( 1000 );
    }
    else {
	throw std::runtime_error( "Please specify a modern dict as command line option (--modernDict=some.fbdic)" );
    }

    // set a historical dictionary
    if( options.hasOption( "histDict" ) ) {
	csl::DictSearch::DictModule& historicMod = dictSearch.addDictModule( L"histCorpus", options.getOption( "histDict" ) );
	historicMod.setDLevWordlengths();
	historicMod.setPriority( 90 );
    }
    
    // set a NE dictionary
    if( options.hasOption( "NEDict" ) ) {
	csl::DictSearch::DictModule& NEMod = dictSearch.addDictModule( L"NamedEnt", options.getOption( "NEDict" ) );
	NEMod.setDLevWordlengths();
	NEMod.setPriority( 90 );
    }
    
    std::wstring query;
    csl::DictSearch::CandidateSet candSet;

    bool machineReadable = false;
    if( options.hasOption( "machineReadable" ) ) {
	machineReadable = true;
    }

    
    while( std::getline( std::wcin, query ).good() ) {
	candSet.clear(); // empty the CandidateSet
	dictSearch.query( query, &candSet ); // execute lookup
	
#ifndef CSL_DICTSEARCH_PRINTNONE
	std::sort( candSet.begin(), candSet.end() ); // sort candidates following a very coarse order relation
	if( candSet.empty() ) {
	    std::wcout<<"--NONE--"<<std::endl;
	}
	else if( machineReadable ) {
	    // all interpretations of the query in one line
	    for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
		std::wcout << *it;
		if( it + 1  != candSet.end() ) std::wcout<<"|";
	    }
	    std::wcout<<std::endl;
	}
	else {
	    for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
		std::wcout << *it << std::endl;

		continue;

		std::wcout <<  it->getWord() << std::endl;
		std::wcout <<  "  baseWord="  << it->getBaseWord() << std::endl;
		std::wcout <<  "  intruction="  << it->getInstruction() << std::endl;
		std::wcout <<  "  levDistance="  << it->getLevDistance() << std::endl;
		std::wcout <<  "  dict="  << it->getDictModule().getName() << std::endl;
		std::wcout << std::endl;
	    }
	}
#endif
    }
    
}
