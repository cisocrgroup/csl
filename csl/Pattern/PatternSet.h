#ifndef CSL_PATTERN_SET
#define CSL_PATTERN_SET CSL_PATTERN_SET

#include<fstream>
#include<vector>

#include "../Global.h"
#include "./Pattern.h"


namespace csl {

    /**
     * @brief A collection of patterns that should be available in some context.
     *
     * @author Ulrich Reffle, 2008
     */
    class PatternSet {

    public:
	/**
	 * @brief A \c PatternRef -object serves as pointer to a distinct member of
	 * a \c PatternSet.
	 * 
	 * The important property of this kind of pointer is that it never expires
	 * as long as the \c PatternSet -object lives. 
	 */
	class PatternRef {
	public:
	    friend class PatternSet;
	
	    /**
	     * @brief get the left side of the pattern
	     * @return the left side of the pattern
	     */
	    const std::wstring& getLeft() const {
		return patternSet_.patternList().at( index_ ).getLeft();
	    }
	
	    /**
	     * @brief get the right side of the pattern
	     * @return the right side of the pattern
	     */
	    const std::wstring& getRight() const {
		return patternSet_.patternList().at( index_ ).getLeft();
	    }

	    /**
	     * @brief returns if pattern is "empty"
	     */
	    bool empty() const {
		return patternSet_.patternList().at( index_ ).empty();
	    }

	    void print( std::wostream& os = std::wcout ) const {
		patternSet_.patternList().at( index_ ).print( os );
	    }

	private:
	    /**
	     * @brief Constructs a new PatternRef to the \c i-th position of \c patternSet
	     */
	    PatternRef( const PatternSet& patternSet, size_t i ) :
		patternSet_( patternSet ),
		index_( i ) {
	    
	    }

	    const PatternSet& patternSet_;
	    size_t index_;

	}; // class PatternRef


	/**
	 * @brief 
	 */
	PatternSet();

	/**
	 * @brief Returns a \c PatternRef object pointing to the \c i -th position of the
	 * set
	 */
	PatternRef at( size_t i ) const {
	    return PatternRef( *this, i );
	}
	
	

	/**
	 * @brief Reads a set of patterns from file \c patternFile.
	 *
	 * The file is expected to contain one pattern per line, with a blank to
	 * separate left from right side.
	 */
	void loadPatterns( const char* patternFile );

    protected:
	typedef std::vector< Pattern > PatternList_t;

	Pattern emptyPattern_;
	const PatternList_t& patternList() const {
	    return patternList_;
	}

    private:
	PatternList_t patternList_;
    }; // class PatternSet


    PatternSet::PatternSet() :
	emptyPattern_( Pattern() ) {
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
	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( "PatternGraph: Invalid line in pattern file" );
	    }

	    patternList_.push_back( Pattern( line.substr( 0, delimPos ), line.substr( delimPos + 1 ) ) );
	}
    } // PatternSet::loadPatterns


} // eon


#endif
