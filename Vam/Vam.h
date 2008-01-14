#include<iostream>

#include<vector>
#include<stack>
#include<set>

#include "../Global.h"
#include "../Stopwatch.h"
#include "../MinDic/MinDic.h"
#include "../LevDEA/LevDEA.h"

namespace csl {

    class Vam {
    public:
	Vam( const char* patternFile );

    private:
	void loadPatterns( const char* patternFile );
	
    };

    Vam::Vam( const char* patternFile ) {
	loadPatterns( patternFile );
    }


 

    void Vam::loadPatterns( const char* patternFile ) {
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
    
    void Vam::query( std::wstring& word, std::vector< Answer >* answers ) {
	query_ = word;
	answers_ = answers;
	
	usedPatterns_.clear();
	query_rec( original_, 0 );
    }

    void query_rec( std::wstring word, size_t left ) {
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
	
    } // query_rec



} // namespace csl
