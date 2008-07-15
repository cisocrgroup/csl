#ifndef CSL_PATTERN_SET
#define CSL_PATTERN_SET CSL_PATTERN_SET

#include<fstream>
#include<vector>
#include<errno.h>

#include "../Global.h"
#include "./Pattern.h"


namespace csl {

    /**
     * @brief A collection of patterns that should be available in some context.
     *
     * @author Ulrich Reffle, 2008
     */
    class PatternSet {
    protected:
	typedef std::vector< Pattern > PatternList_t;

    public:
	typedef PatternList_t::iterator iterator;
	typedef PatternList_t::const_iterator const_iterator;


	/**
	 * @brief 
	 */
	PatternSet();


	/**
	 * @brief Returns a reference pointing to the \c i -th position of the
	 * set
	 */
	Pattern const& at( size_t i ) const {
	    return patternList_.at( i );
	}


	/**
	 * @brief returns an iterator at the first pattern of the list
	 *
	 * Note that this is not the necessarily the first element of the internal container
	 */
	iterator begin() {
	    return ++( patternList_.begin() );
	}
	
	/**
	 * @brief returns a classical past-the-end iterator
	 */
	iterator end() {
	    return patternList_.end();
	}
	

	/**
	 * @brief Reads a set of patterns from file \c patternFile.
	 *
	 * The file is expected to contain one pattern per line, with a blank to
	 * separate left from right side.
	 */
	void loadPatterns( const char* patternFile );

    protected:

	const PatternList_t& patternList() const {
	    return patternList_;
	}

    private:
	PatternList_t patternList_;
    }; // class PatternSet


    PatternSet::PatternSet() {
	patternList_.push_back( Pattern() );
    }

    void PatternSet::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	if( ! fi ) {
	    throw exceptions::badFileHandle( "PatternSet::Could not open pattern file" );
	}
	
	std::wstring line;

	size_t patternCount = 0;
	while( getline( fi, line ) ) {
	    if( errno == EILSEQ ) { // if failbit is set BEFORE eof
		throw exceptions::badInput( "PatternSet::loadPatterns: Encoding error in input sequence." );
	    }

	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( "PatternGraph: Invalid line in pattern file" );
	    }

	    patternList_.push_back( Pattern( line.substr( 0, delimPos ), line.substr( delimPos + 1 ) ) );
	}
    } // PatternSet::loadPatterns


} // eon


#endif
