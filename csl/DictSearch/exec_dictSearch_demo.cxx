#include<locale>
#include <csl/DictSearch/DictSearch.h>


int main() {

    std::locale::global( std::locale( "" ) );

    csl::DictSearch::DictSearch dictSearch;

    dictSearch.getConfigModern().setDict( "../csl/DictSearch/Test/small.modern.fbdic" );
    dictSearch.getConfigHistoric().setDict( "../csl/DictSearch/Test/small.historical.fbdic" );
    dictSearch.initHypothetic( "../csl/DictSearch/Test/small.patterns.txt" );

    csl::DictSearch::CandidateSet candSet;
    dictSearch.query( L"theile", &candSet );
    std::sort( candSet.begin(), it != candSet.end() );

    for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
	std::wcout <<  *it << std::endl;
    }

    return 0;
}
