#include<locale>
#include <csl/DictSearch/DictSearch.h>


int main() {

    std::locale::global( std::locale( "" ) );

    // create a DictSearch-object
    csl::DictSearch dictSearch;
    // set a modern dictionary
    dictSearch.getConfigModern().setDict( "../csl/DictSearch/Test/small.modern.fbdic" );
    // configure approx. search on modern dict. with distance bound 2
    dictSearch.getConfigModern().setDLev( 2 );
    
    // set a historical dictionary
    dictSearch.getConfigHistoric().setDict( "../csl/DictSearch/Test/small.historical.fbdic" );
    // configure approx. search on modern dict. to choose default distance bounds according to the word length
    dictSearch.getConfigHistoric().setDLevWordlengths();
    // initialise the hypothetic dict. with a file of patterns
    dictSearch.initHypothetic( "../csl/DictSearch/Test/small.patterns.txt" );

    std::wstring query;
    csl::DictSearch::CandidateSet candSet;

    while( std::getline( std::wcin, query ).good() ) {
	candSet.clear(); // empty the CandidateSet
	dictSearch.query( query, &candSet ); // execute lookup
	std::sort( candSet.begin(), candSet.end() ); // sort candidates following a very coarse order relation
	
	for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
	    std::wcout << *it << std::endl; continue;
	    std::wcout <<  it->getWord() << std::endl;
	    std::wcout <<  "  baseWord="  << it->getBaseWord() << std::endl;
	    std::wcout <<  "  intruction="  << it->getInstruction() << std::endl;
	    std::wcout <<  "  levDistance="  << it->getLevDistance() << std::endl;
	    std::wcout <<  "  dict="  << it->getDictID_string() << std::endl;
	    std::wcout << std::endl;
	}
    }
    
}
