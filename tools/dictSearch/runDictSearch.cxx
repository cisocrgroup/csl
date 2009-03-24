#include <locale>
#include <csl/DictSearch/DictSearch.h>
#include <csl/Getopt/Getopt.h>

// uncomment this if you want so suppress output for the words
//#define CSL_DICTSEARCH_PRINTNONE 1;

int main( int argc, char const** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    Getopt options( argc, argv );

    if( options.getArgumentCount() != 3 ) {
	std::wcerr<<std::endl
		  << "Use like: dictSearch <historicalDict> <modernDict> <patternFile> "<<std::endl
		  << "Options:"<<std::endl
		  << "No options so far."
		  <<std::endl<<std::endl;
	
	exit(1);
    }
    
    // create a DictSearch-object
    csl::DictSearch dictSearch;

    // initialise the hypothetic dict. with a file of patterns
    dictSearch.initHypothetic( options.getArgument( 2 ).c_str() );

    // set a modern dictionary
    csl::DictSearch::DictModule& modernMod = dictSearch.addDictModule( L"modern", options.getArgument( 1 ) );
    modernMod.setDLevWordlengths();
    modernMod.setPriority( 100 );
    modernMod.setMaxNrOfPatterns( 1000 );
    
    // set a historical dictionary
    csl::DictSearch::DictModule& historicMod = dictSearch.addDictModule( L"historic", options.getArgument( 1 ) );
    historicMod.setDLevWordlengths();
    historicMod.setPriority( 90 );
    
    std::wstring query;
    csl::DictSearch::CandidateSet candSet;
    
    while( std::getline( std::wcin, query ).good() ) {
	candSet.clear(); // empty the CandidateSet
	dictSearch.query( query, &candSet ); // execute lookup
	
#ifndef CSL_DICTSEARCH_PRINTNONE
	std::sort( candSet.begin(), candSet.end() ); // sort candidates following a very coarse order relation
	for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
	    std::wcout << *it << std::endl; continue;
	    std::wcout <<  it->getWord() << std::endl;
	    std::wcout <<  "  baseWord="  << it->getBaseWord() << std::endl;
	    std::wcout <<  "  intruction="  << it->getInstruction() << std::endl;
	    std::wcout <<  "  levDistance="  << it->getLevDistance() << std::endl;
	    std::wcout <<  "  dict="  << it->getDictModule().getName() << std::endl;
	    std::wcout << std::endl;
	}
#endif
    }
    
}
