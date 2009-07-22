#ifndef CSL_PATTERNWEIGHTS_H
#define CSL_PATTERNWEIGHTS_H CSL_PATTERNWEIGHTS_H

#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<map>
#include<math.h>
#include<csl/Global.h>
#include<csl/Pattern/Pattern.h>

namespace csl {

    /**
     *
     * @author Ulrich Reffle <uli@cis.uni-muenchen.de>
     * @date 2009
     */
    class PatternWeights {
    public:
	static const int UNDEF = -1;

	typedef std::map< csl::Pattern, float >::iterator PatternIterator;
	typedef std::map< csl::Pattern, float >::const_iterator const_PatternIterator;

	/**
	 * @brief a PatternType describes the relation of the lengths of both pattern sides.
	 *        E.g.: the pattern t->th would have the type 1:2
	 *
	 * Those types are used to specify standard weights for certain pattern types
	 */
	class PatternType : public std::pair< size_t, size_t > {
	public:
	    PatternType( size_t l, size_t r ) {
		first = l;
		second = r;
	    }
	};

	PatternWeights();

	void reset();

	void clear();

	/**
	 * @return the weight for a given pattern, or PatternWeights::UNDEF if the pattern is not known
	 */
	float getWeight( const  csl::Pattern& pattern ) const;

	/**
	 * @brief set a new weight for a pattern
	 */
	void setWeight( const csl::Pattern& pattern, float weight );



	float getDefault( PatternType const& patternType ) const;   


	void setDefault( PatternType const& patternType, float weight );


	/**
	 * @brief If "smartMerge" is activated, it will override the default settings
	 *        (set via setDefault()) and return the weight UNDEF for 
	 *        all merges and splits of the form "xy <-> x" and "xy <-> y".
	 *        It does NOT override the specific weights added via setWeight()
	 * 
	 */
	void setSmartMerge( bool t = true );


	const_PatternIterator patternsBegin() const {
	    return patternWeights_.begin();
	}
	
	PatternIterator patternsBegin() {
	    return patternWeights_.begin();
	}
	
	const_PatternIterator patternsEnd() const {
	    return patternWeights_.end();
	}
	
	PatternIterator patternsEnd() {
	    return patternWeights_.end();
	}
	

	/**
	 * @brief prints the pattern weights
	 */
	void print( std::wostream& str = std::wcout ) const;
	

	void loadFromFile( const char* patternFile );	
	void writeToFile( const char* patternFile ) const;	

	private:

	/**
	 * @brief This data structure maps the known patterns to their weights
	 */
	std::map< csl::Pattern, float> patternWeights_;

	/**
	 *
	 */
	std::map< PatternType, float > defaultWeights_;

	bool smartMerge_;

	/**
	 * @brief sorts the std::pairs according to their second value
	 */
	template< typename pair_t >
	static bool sortBySecond( pair_t const& a, pair_t const& b ) {
	    return ( a.second > b.second ); // reverse ordering
	}
	
   }; // class PatternWeights



} // ns csl

#endif
