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
	inline PatternSet();


	/**
	 * @brief Returns a reference pointing to the \c i -th position of the
	 * set
	 */
	inline Pattern const& at( size_t i ) const {
	    return patternList_.at( i );
	}

	/**
	 * @brief Returns a reference pointing to the \c i -th position of the
	 *        STRIPPED set. 
	 *
	 */
	inline Pattern const& stripped_at( size_t i ) const {
	    return strippedPatternList_.at( i );
	}


	/**
	 * @brief returns an iterator at the first pattern of the list
	 *
	 * Note that this is not the necessarily the first element of the internal container
	 */
	inline iterator begin() {
	    return ++( patternList_.begin() );
	}
	
	/**
	 * @brief returns a classical past-the-end iterator
	 */
	inline iterator end() {
	    return patternList_.end();
	}
	

	/**
	 * @brief Reads a set of patterns from file \c patternFile.
	 *
	 * The file is expected to contain one pattern per line, with a blank to
	 * separate left from right side. Example:
	 * @code
	 * t th
	 * ei ey
	 * u v
	 * @endcode
	 */
	inline void loadPatterns( const char* patternFile );

    protected:

	inline const PatternList_t& patternList() const {
	    return patternList_;
	}

    private:
	PatternList_t patternList_;

	/**
	 * @brief This list holds all patterns which are also in patternList_, but WITHOUT the ^ and $ markers.
	 */
	PatternList_t strippedPatternList_;
    }; // class PatternSet


    PatternSet::PatternSet() {
	patternList_.push_back( Pattern() );
	strippedPatternList_.push_back( Pattern() );
    }

    void PatternSet::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( CSL_UTF8_LOCALE );
	fi.open( patternFile );
	if( ! fi.good() ) {
	    std::string message = 
		std::string( "PatternSet::Could not open pattern file: " ) +
		patternFile;
		
	    throw exceptions::badFileHandle( message );
	}
	
	std::wstring line;

	size_t patternCount = 0;
	while( getline( fi, line ).good() ) {

	    if( line.empty() ) continue;
	    if( line.at(0) == '#' ) continue;
	    
	    size_t delimPos = line.find( Pattern::leftRightDelimiter_ );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( std::string( "PatternSet: Invalid line in pattern file: " ) + csl::CSLLocale::wstring2string( line ) );
	    }

	    std::wstring left = line.substr( 0, delimPos );
	    std::wstring right = line.substr( delimPos + 1 );
	    patternList_.push_back( Pattern( left, right ) );

	    if( (! left.empty()) && ( left.at( 0 ) == Global::wordBeginMarker ) ) left.erase( 0, 1 );
	    if( (! left.empty()) && ( left.at( left.size()-1 ) == Global::wordEndMarker ) ) left.erase( left.size()-1, 1 );
	    strippedPatternList_.push_back( Pattern( left, right ) );

	    
	}
	if( errno == EILSEQ ) { // catch encoding error
	    throw exceptions::badInput( "csl::PatternSet: Encoding error in input sequence." );
	}

	std::wcerr<<"csl::PatternSet: Loaded "<<patternList_.size() - 1 << " patterns." << std::endl;
    } // PatternSet::loadPatterns


} // eon


#endif
