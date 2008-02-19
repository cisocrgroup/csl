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
    class PatternSet : public std::vector< Pattern > {
    public:
	/**
	 * @brief Reads a set of patterns from a file.
	 *
	 * The file is expected to contain one pattern per line, with a blank to
	 * separate left from right side.
	 */
	void loadPatterns( const char* patternFile );
    private:
	
    };

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

	    this->push_back( Pattern( line.substr( 0, delimPos ), line.substr( delimPos + 1 ) ) );
	}
    }


} // eon


#endif
