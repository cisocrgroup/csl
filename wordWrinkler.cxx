#include<iostream>
#include "./MinDic/MinDic.h"
#include "./Stopwatch.h"

std::wstring indent( size_t depth ) {
    return std::wstring( depth, L'.' );
}

class WordWrinkler {
public:

    struct Answer {
	std::wstring word;
	std::vector< std::pair< std::wstring, std::wstring > > patterns;
    };

    WordWrinkler( const char* patternFile ) {
	loadPatterns( patternFile );
    }
    
    void wrinkle( std::wstring& word, std::vector< Answer >* answers ) {
	original_ = word;
	answers_ = answers;
	
	usedPatterns_.clear();
	wrinkle_rec( original_, 0 );
    }

private:
    typedef csl::MinDic<>::State MDState_t;

    typedef std::vector< std::wstring > RightList_t;

    void wrinkle_rec( std::wstring word, size_t left ) {
	// std::wcout<<"Enter wrinkle_rec with word="<<word<<", left="<<left<<std::endl;
	
	size_t wlength = word.length();
	
	bool foundLeftSide = false;
	MDState_t patState( leftSides_ );
	
	size_t offset = 0;
	while( left < wlength && ! foundLeftSide ) {
	    // std::wcout<<"Try with left="<<left<<std::endl; // DEBUG
	    patState = leftSides_.getRootState();
	    offset = 0;
	    while( ! foundLeftSide && ( left + offset < wlength ) && patState.walk( word.at( left + offset ) ) ) { 
		if( patState.isFinal() ) {
		    foundLeftSide = true;
		}
		else ++offset;
	    } // while can walk
	    if( ! foundLeftSide ) ++left;
	} // while left is shifted

	if( foundLeftSide ) {
	    wrinkle_rec( word, left + 1 );
	    // std::wcout<<"Come back. word="<<word<<", left="<<left<<std::endl; // DEBUG
	    
	    do {
		if( patState.isFinal() ) {
		    const RightList_t& rights = rightSides_.at( patState.getAnnotation() );
		    for( RightList_t::const_iterator it = rights.begin(); it != rights.end(); ++it ) {
			// std::wcout<<"Here is a right side for left side "<<patState.getAnnotation()<<":"<<*it<<std::endl; // DEBUG
			std::wstring newWord( word );
			newWord.replace( left, offset + 1, *it );
			
			usedPatterns_.push_back( std::pair< std::wstring, std::wstring >( word.substr( left, offset + 1 ), *it ) );
			wrinkle_rec( newWord, left + it->length() );
			// std::wcout<<"Come back. word="<<word<<", left="<<left<<std::endl;// DEBUG
			usedPatterns_.resize( usedPatterns_.size() - 1 );
		    }
		}
		++offset;
	    } while( ( left + offset < wlength ) && patState.walk( word.at( left + offset ) ) );

	}


	if( ( wlength == left ) && ! usedPatterns_.empty() ) {
	    Answer answer;

	    // std::wcout<<">>"<<word<<":";   // DEBUG
	    answer.word = word;
	    for( std::vector< std::pair< std::wstring, std::wstring > >::const_iterator it = usedPatterns_.begin();
		 it != usedPatterns_.end(); 
		 ++it ) {
		// std::wcout<< "(" << it->first <<"->"<< it->second << ")"; // DEBUG
		answer.patterns.push_back( *it );
	    }
	    // std::wcout<<std::endl;  // DEBUG

	    answers_->push_back( answer );
	}
	
    } // wrinkle_rec

    void loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	if( ! fi ) {
	    throw csl::exceptions::badFileHandle( "WordWrinkler::Could not open pattern file" );
	}
	
	typedef std::map< std::wstring, RightList_t > PatternMap_t;
	PatternMap_t patterns;
	std::wstring line;

	while( getline( fi, line ) ) {
	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw csl::exceptions::badInput( "ErrDicConstructor: Invalid line in pattern file" );
	    }
	    std::wstring left = line.substr( 0, delimPos );
	    std::wstring right = line.substr( delimPos + 1 );
	    
	    patterns[left].push_back( right );
	}
	fi.close();

	leftSides_.initConstruction();

	for( PatternMap_t::const_iterator it = patterns.begin(); it != patterns.end(); ++it ) {

// 	    std::wcout<<it->first<<"->";
// 	    for( RightList_t::const_iterator rit = it->second.begin(); rit != it->second.end(); ++rit ) {
// 		std::wcout<<*rit<<" | ";
// 	    }
// 	    std::wcout<<std::endl;

	    // here we copy the whole vector, but who cares ....
	    rightSides_.push_back( it->second );

	    leftSides_.addToken( it->first.c_str(), rightSides_.size()-1 ); // annotate the proper index into rightSides_
	}
	leftSides_.finishConstruction();
	// leftSides_.toDot(); // DEBUG
    }

    std::wstring original_;

    std::vector< Answer >* answers_;
    
    std::vector< std::pair< std::wstring, std::wstring > > usedPatterns_;

    csl::MinDic< int > leftSides_;
    std::vector< std::vector< std::wstring > > rightSides_;

}; // class wordWrinkler

int main( int argc, const char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    Stopwatch watch;
    watch.start();

    WordWrinkler ww( argv[1] );

    size_t nrOfParadigms = 0;
    size_t nrOfCookedParadigms = 0;
    std::wstring paradigm;
    while( std::wcin >> paradigm ) {
	// comment lines beginning with '#' are handed on to the output
	if( paradigm.at( 0 ) == L'#' ) {
	    std::wcout<<paradigm<<std::endl;
	    continue;
	}

	++nrOfParadigms;
	std::vector< std::wstring > forms;
	size_t begin = 0;
	size_t sepPos = 0;
	while( ( sepPos = paradigm.find_first_of( ',', begin ) ), sepPos != std::string::npos ) {
	    forms.push_back( paradigm.substr( begin, sepPos - begin ) );
	    begin = sepPos + 1;
	}
	forms.push_back( paradigm.substr( begin ) );
	
	
	// find longest prefix and store suffixes
	std::vector< std::wstring > suffixes;

	std::wstring prefix = forms.at( 0 );
	for( std::vector< std::wstring >::const_iterator it = forms.begin(); it != forms.end(); ++it ) {
	    size_t pos = 0;
	    while( pos < prefix.length() && pos < it->length() && prefix.at( pos ) == it->at( pos ) ) {
		++pos;
	    }
	    prefix.resize( pos );
	}

	for( std::vector< std::wstring >::const_iterator it = forms.begin(); it != forms.end(); ++it ) {
	    suffixes.push_back( it->substr( prefix.length() ) );

	}


//	std::wcout<<"Prefix: "<<prefix<<", suffixes: ";// DEBUG
// 	for( std::vector< std::wstring >::const_iterator sufIt = suffixes.begin(); sufIt != suffixes.end(); ++sufIt ) { // DEBUG
// 	    std::wcout<<"-"<<*sufIt<<", "; // DEBUG
// 	} // DEBUG
// 	std::wcout<<std::endl; // DEBUG
	
	
	std::vector< WordWrinkler::Answer > answers;
	ww.wrinkle( prefix, &answers );

	for( std::vector< WordWrinkler::Answer >::const_iterator wrinkleIt = answers.begin(); wrinkleIt != answers.end(); ++wrinkleIt ) {
	    ++nrOfCookedParadigms;
	    for( std::vector< std::wstring >::const_iterator sufIt = suffixes.begin(); sufIt != suffixes.end(); std::wcout<<",", ++sufIt ) {
		std::wcout<<wrinkleIt->word<<*sufIt;
	    }
	    std::wcout<<std::endl;
	}
    } // for all paradigms from stdin
    
    std::wcerr<<"Cooked "<<nrOfCookedParadigms<<" hypothetical paradigms out of "<<nrOfParadigms<<" real ones. Time: "<<watch.readMilliseconds()<<" milliseconds"<<std::endl;
}


