/**
@page dictSearch_manual DictSearch Manual - \n Approximate Search in Dictionaries in the Context of Historical Language

@section DictSearch_general General Notes
csl::DictSearch is a combined interface for approximate dictionary lookup in the context of historical language. The scenario is that the users
specify a modern as well as a historical dictionary to perform exact or approximate dictionary lookups.
A third component is what we call the "hypothetic dictionary", containing all orthographical variants that
can possibly be derived from some word of the modern dictionary and the application of some "orthographical
variant patterns". These patterns are simple rewrite rules and can be specified by the user.

For a query word @c w the users receive as answer a set of words, containing exact and approximate matches for 
either of the three dictionaries. In the usual application this answer set is understood as set of correction
candidates for a (probably garbled) token @c w. The task of ranking these candidates is explicitly out of the scope of 
csl::DictSearch: To decide for a correction candidate, various other techniques for channel and language modelling have 
to be taken into account.

This module is under construction. Please do help to extend the 'Known Issues' section at the end of this page.
Also, this manual is work in progress. For now it concentrates on practical issues for the usage of the module. Some more details 
about the algorithms that were used will be added.

@section example_io Example: Input/Output
Let's start with an example, showing what the module offers. We load a very small modern dictionary with the following words and attached frequency scores:
@code
(feile#42, teile#84, teilen#101, teller#3, verteilen#18)
@endcode
We also configure csl::DictSearch to load a small historical dictionary (no frequency scores attached):
@code
(theil, theile, theller, theyl, theyle)
@endcode
The hypothetic dictionary shall contain all variants of words from the modern dictionary, using the following variant patterns:
@code
t->th, ei->ey. u->v
@endcode
Finally, let's allow approximate matching only for the modern dictionary, with distance threshold 2.

Here's what DictSearch returns for the query "theile":
@code
Query: theile
Output:
theile:theile+[],dist=0,baseWordScore=0(historic)
theile:teile+[(t_th,0)],dist=0,baseWordScore=84(modern)
teile:teile+[],dist=1,baseWordScore=84(modern)
theil:theil+[],dist=1,baseWordScore=0(historic)
theyle:theyle+[],dist=1,baseWordScore=0(historic)
feile:feile+[],dist=2,baseWordScore=42(modern)
teilen:teilen+[],dist=2,baseWordScore=101(modern)
@endcode
The meaning of all components can be seen best when investigating the 2nd answer:
@code
theile:teile+[(t_th,0)],dist=0,baseWordScore=84(modern)
@endcode
- 'theile' is the candidate string itself.
- It is derived from 'teile' using a variant pattern 't -> th' on position 0. This tells us the word is taken from the hypothetic dictionary.
- The exact string was found in the dictionary, thus the levenshtein distance is 0.
- The baseword (on which the variant pattern was applied) comes from the modern dictionary and had a score '84' attached.


@section installation Compilation/ Installation
Please read the INSTALL file in the project root directory. The make command will compile a whole lot of things you don't need, but never mind.
The open source program 'cmake' produces Makefiles for Linux and vcproj-files for MS Visual C++. On Windows, The project compiles, but with lots of warnings.  At the moment, there's a major bug with Visual Studio 2008 connected to utf8 input. The problem does not exist with Visual Studio 2005.

@section fbdic Dictionary Format: fbdic
To use a dictionary with csl::DictSearch, it has to be compiled into a finite-state automaton. csl::FBDic is responsible for that.
Please read in the @link fbDic_manual FBDic Manual@endlink how to get FBDic automata.

There are multiple ways to connect a dictionary to DictSearch:
- create binary files of the automata in an offline step using the tool compileFBDic (recommended for large dictionaries)
  - and then simply pass on the filename to the DictSearch configuration
  - or load the file into a csl::FBDic object and pass a reference of it to the DictSearch configuration
- To create the automata at runtime using csl::FBDic::compileDic()

@subsection fbdic_size On the size of fbdics
Unfortunately the resulting data structure is pretty large ( less than 62MB for a German dictionary of 2.9 million words) - that's because the approximate matching algorithm actually needs two variants of the automaton simultaneously in the memory. A slower variant of the search algorithm gets along with dictionaries of half the size.


@section configuration Configuration
The parameters that affect the candidate set are obvious: For the modern and for the historical dictionary, the result
depends on the choice of the dictionaries themselves, and the upper bound of edit operations that are allowed for 
approximate search. In practice it makes sense to allow more edit operations for longer words - so the distance bound
can be set in relation to the length of the query word. For the hypothetical dictionary, additional parameters are the set
of orthographical variants and an upper bound for the number of applied variant patterns.

For configuration of the lookup in modern and historical dictionaries, DictSearch provides configuration objects via the 
following two methods:
- csl::DictSearch::ConfigLookup& csl::DictSearch::getConfigModern()
- csl::DictSearch::ConfigLookup& csl::DictSearch::getConfigHistoric()

Please consult the class reference of csl::DictSearch::ConfigLookup or the demo program below for details on how to set
the above mentioned parameters. 

@subsection hypothetic_configuration Initialising and configuring the hypothetic dictionary
The hypothetic dictionary uses a slightly extended object for configuration:
- csl::DictSearch::ConfigHypothetic& csl::DictSearch::getConfigHypothetic()

Furthermore the hypothetic dictionary must be initialised separately, using the method csl::DictSearch::initHypothetic( char const* patternFile ).
This method will activate the hypothetic dictionary, and its configuration can be changed as explained above.

Here is an example @c patternFile, the first line specifies that 't' might be spelled 'th' in historic documents. Use a simple <SPACE> as delimiter.
@code
t th
ei ey
u v
@endcode

This code-snippet shows a very simple way to initialise and configure a DictSearch. A more detailed example can be found at the bottom
of this page.
@code
    // create a DictSearch-object
    csl::DictSearch dictSearch;
    // set a modern dictionary
    dictSearch.getConfigModern().setDict( "path-to/some/modern.fbdic" );
    // configure approx. search on modern dict. with distance bound 2
    dictSearch.getConfigModern().setDLev( 2 );
    
    // set a historical dictionary
    dictSearch.getConfigHistoric().setDict( "path-to/some/historical.fbdic" );
    // configure approx. search on modern dict. to choose default distance bounds according to the word length
    dictSearch.getConfigHistoric().setDLevWordlengths();
    // initialise the hypothetic dict. with a file of patterns
    dictSearch.initHypothetic( "path-to/some/patterns.txt" );
@endcode


@section lookup Lookup
Different dictionaries, orthographic variants, approximate lookup: The answer to a DictSearch-query is more complex than it is for conventional 
fuzzy dictionary lookup. We hope we have found a way to keep things simple if the user is not interested in the details, and yet to keep things
clear even if maybe the left side of the 3rd variant pattern of the 16th candidate is of special interest.

The method csl::DictSearch::query( std::wstring const& word, csl::DictSearch::CandidateSet* candSet ) works with an input and an output argument.
- csl::DictSearch::Interpretation, an extension of class csl::Interpretation - holds one candidate.
- csl::DictSearch::CandidateSet is a container that holds a set of such interpretations. It is comparable to (in fact, implemented as) a std::vector< csl::DictSearch::Interpretation >, providing the standard access methods like begin(), end(), at() (see 
the class reference).

This code-snippet shows a very simple way to query DictSearch. A more detailed example can be found at the bottom of this page.
@code
std::wstring query;
csl::DictSearch::CandidateSet candSet;

while( std::getline( std::wcin, query ).good() ) {
   candSet.clear(); // empty the CandidateSet
   dictSearch.query( query, &candSet ); // execute lookup
   std::sort( candSet.begin(), candSet.end() ); // sort candidates following a very coarse order relation
	
   for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
      std::wcout <<  it->getWord() << std::endl;
      std::wcout <<  "  baseWord="  << it->getBaseWord() << std::endl;
      std::wcout <<  "  intruction="  << it->getInstruction() << std::endl;
      std::wcout <<  "  levDistance="  << it->getLevDistance() << std::endl;
      std::wcout <<  "  dict="  << it->getDictID_string() << std::endl;
      std::wcout << std::endl;
   }
}
@endcode

@subsection interpretation Investigating single candidates (of type csl::DictSearch::Interpretation)
csl::DictSearch::Interpretation and its base class csl::Interpretation provide lots of information about the candidate they describe.
Here are the most useful ones (they are also used in the example above):
- csl::Interpretation::getWord() returns the plain string form of the candidate
- csl::Interpretation::getBaseWord() If the candidate is part of the hypothetical dictionary, here is the modern word it was derived from
- csl::Interpretation::getInstruction() returns an object of type csl::Instruction, storing the applied variant patterns
- csl::Interpretation::getLevDistance() returns the levenshtein distance to the query
- csl::Interpretation::operator<() defines a very coarse order relation, so the candidates can be pre-sorted using std::sort. The user
    can easily overrule this sorting order by passing a custom-made sort operator to std::sort.


@section example_program Example: Program Code
This program code is part of the software package: csl/DictSearch/exec_dictSearch_demo.cxx  . 'make dictSearch_demo' builds the executable.
@code
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
	    std::wcout <<  it->getWord() << std::endl;
	    std::wcout <<  "  baseWord="  << it->getBaseWord() << std::endl;
	    std::wcout <<  "  intruction="  << it->getInstruction() << std::endl;
	    std::wcout <<  "  levDistance="  << it->getLevDistance() << std::endl;
	    std::wcout <<  "  dict="  << it->getDictID_string() << std::endl;
	    std::wcout << std::endl;
	}
    }
    
}
@endcode

@section known_issues Known Bugs/Issues
- On Windows we're working on a bug related to the decoding of utf-8 files. Programs will crash if you use anything but ascii characters. It works fine under linux, and soon it will work fine for Windows, too.
- We're aware that Abbyy might be interested in a feature that is more tailor-made to those "fuzzy" word graphs used inside the Abbyy engine. We intend to put effort in that direction.

*/
