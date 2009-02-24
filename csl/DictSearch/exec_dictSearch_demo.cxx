#include<locale>
#include <csl/DictSearch/DictSearch.h>


int main() {

    std::locale::global( std::locale( "" ) );

    try {

    // create a DictSearch-object
    csl::DictSearch dictSearch;
    // set a modern dictionary
//    csl::DictSearch::DictModule& modernDic = dictSearch.addDictModule( L"modern", std::string( "../csl/DictSearch/Test/small.modern.fbdic" ) );
    csl::DictSearch::DictModule& modernDic = dictSearch.addDictModule( L"modern", std::string( "/mounts/data/proj/impact/software/Vaam/cislexAll_plus_dCorrect_kompAbschl.fbdic" ) );
    // configure approx. search on modern dict. with distance bound 2
    modernDic.setDLev( 2 );
    modernDic.setPriority( 2 );
    
    // set a historical dictionary
//    csl::DictSearch::DictModule& histDic = dictSearch.addDictModule( L"hist", std::string( "../csl/DictSearch/Test/small.historical.fbdic" ) );
    csl::DictSearch::DictModule& histDic = dictSearch.addDictModule( L"hist", std::string( "/mounts/Users/student/uli/implement/cxx/ocr_analysis/historic.fbdic" ) );
    histDic.setPriority( 1 );
    // configure approx. search on historic dict. to choose default distance bounds according to the word length
    histDic.setDLevWordlengths();

    // initialise the hypothetic dict. with a file of patterns
//    dictSearch.initHypothetic( "../csl/DictSearch/Test/small.patterns.txt" );
    dictSearch.initHypothetic( "/mounts/data/proj/impact/software/Vaam/patterns.lexgui.txt" );

    modernDic.setMaxNrOfPatterns( 15 );

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
	    std::wcout <<  "  dict="  << it->getDictModule().getName() << std::endl;
	    std::wcout << std::endl;
	}
    }

    } catch( csl::exceptions::cslException exc ) {
	std::wcerr<<"dictSearchDemo caught exception: " << exc.what() << std::endl;
	exit( 1 );
    }    
}
