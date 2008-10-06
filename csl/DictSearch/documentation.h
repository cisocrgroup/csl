/**
@page dictSearch_manual DictSearch Manual

@section DictSearch_general General Notes
DictSearch is a combined interface for a number of modules of the package. The scenario is that the users
specify a modern as well as a historical dictionary to perform exact or approximate dictionary lookups.
A third component is what we call the "hypothetic dictionary", containing all orthographical variants that
can possibly be derived from some word of the modern dictionary and the application of some "orthographical
variant patterns". These patterns are simple rewrite rules and can be specified by the user.

For a query word @c w the users receive as answer a set of words, containing exact and approximate matches for 
either of the three dictionaries. In the usual application this answer set is understood as set of correction
candidates for a (probably garbled) token @c w. The task of ranking these candidates is explicitly out of the scope of 
csl::DictSearch. To decide for a correction candidate, various other techniques for channel and language modelling have 
to be taken into account. Nonetheless a simple order relation is pre-defined, based on the number of edit operations and
orthographical variants.

@section fbdic Dictionary Format: fbdic
The dictionaries to be used must be compiled into a finite-state automaton. csl::FBDic is responsible for that.
There are multiple ways to connect a dictionary to DictSearch:
- create binary files of the automata beforehands using the tool compileFBDic (recommended for large dictionaries)
  - and then simply pass on the filename to the DictSearch configuration
  - or load the file into a csl::FBDic object and pass a reference of it to the DictSearch configuration
- To create the automata at runtime using csl::FBDic::compileDic()

@subsection fbdic_size On the size of fbdics
Unfortunately the resulting data structure is pretty large ( less than 100MB for a dictionary of 2.2 million 
German words) - that's because the approximate matching algorithm actually needs two variants of the automaton
simultaneously in the memory. A slower variant of the search algorithm gets along with dictionaries of half the size.


@section configuration Configuration
The parameters that affect the candidate set are obvious: For the modern and for the historical dictionary, the result
depends on the choice of the dictionaries themselves, and the upper bound of edit operations that are allowed for 
approximate search. In practice it makes sense to allow more edit operations for longer words - so the distance bound
can be set in relation to the length of the query word.

For the hypothetical dictionary


@section example Example 
@code
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
@endcode


*/
