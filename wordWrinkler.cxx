#include<iostream>
#include"./MinDic/MinDic.h"

std::wstring indent( size_t depth ) {
    return std::wstring( depth, L'.' );
}

class WordWrinkler {
public:

    WordWrinkler( const char* patternFile ) {
	loadPatterns( patternFile );
    }
    
    template< typename Collector_t >
    void wrinkle( std::wstring& word, Collector_t& collector ) {
	original_ = word;
	collector_ = collector;
	
	usedPatterns_.clear();
	wrinkle_rec( original_, 0 );
    }

private:
    typedef csl::MinDic<>::State MDState_t;

    typedef std::vector< std::wstring > RightList_t;

    template< typename PatContainer_t >
    void wrinkle_rec( std::wstring word, size_t left, size_t depth = 0 ) {
//	std::wcout<<indent(depth)<<"Enter wrinkle_rec with word="<<word<<", left="<<left<<std::endl;

	size_t wlength = word.length();

	bool foundLeftSide = false;
	MDState_t patState( leftSides_ );
	
	size_t offset = 0;
	while( left < wlength && ! foundLeftSide ) {
//	    std::wcout<<indent( depth )<<"Try with left="<<left<<std::endl;
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
	    wrinkle_rec( word, left + 1, depth + 1 );

	    while( ( left + offset < wlength ) && patState.isValid() ) { 
		if( patState.isFinal() ) {
		    const RightList_t& rights = rightSides_.at( patState.getAnnotation() );
		    for( RightList_t::const_iterator it = rights.begin(); it != rights.end(); ++it ) {
			std::wstring newWord( word );
			newWord.replace( left, offset + 1, *it );
			
			usedPatterns_.push_back( std::pair< std::wstring, std::wstring >( word.substr( left, offset + 1 ), *it ) );
			wrinkle_rec( newWord, left + it->length(), depth + 1 );
			usedPatterns_.resize( usedPatterns_.size() - 1 );
		    }
		}
		patState.walk( word.at( ++offset ) );
	    }
	}


	if( ( wlength == left ) && ! usedPatterns_.empty() ) {
	    collector_.
	    std::wcout<<">>"<<word<<":";
	    for( std::vector< std::pair< std::wstring, std::wstring > >::const_iterator it = usedPatterns_.begin();
		 it != usedPatterns_.end(); 
		 ++it ) {
		std::wcout<< "(" << it->first <<"->"<< it->second << ")";
	    }
	    std::wcout<<std::endl;
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

	    std::wcout<<it->first<<"->";
	    for( RightList_t::const_iterator rit = it->second.begin(); rit != it->second.end(); ++rit ) {
		std::wcout<<*rit<<" | ";
	    }
	    std::wcout<<std::endl;

	    // here we copy the whole vector, but who cares ....
	    rightSides_.push_back( it->second );

	    leftSides_.addToken( it->first.c_str(), rightSides_.size()-1 ); // annotate the proper index into rightSides_
	}
	leftSides_.finishConstruction();
    }

    std::wstring original_;
    Collector_t collector_;

    std::vector< std::pair< std::wstring, std::wstring > > usedPatterns_;

    csl::MinDic< int > leftSides_;
    std::vector< std::vector< std::wstring > > rightSides_;

}; // class wordWrinkler

int main( int argc, const char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */


    WordWrinkler ww( argv[1] );
    std::wstring w;

    while( std::wcin>>w ) {
	ww.wrinkle( w );
    }
}


