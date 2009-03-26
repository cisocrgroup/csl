#ifndef CSL_PATTERNWEIGHTS_H
#define CSL_PATTERNWEIGHTS_H CSL_PATTERNWEIGHTS_H

#include<string>
#include<map>
#include<math.h>
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

	PatternWeights();

	/**
	 * @return the weight for a given pattern, or PatternWeights::UNDEF if the pattern is not known
	 */
	float getWeight( const  csl::Pattern& pattern ) const;

	/**
	 * @brief set a new weight for a pattern
	 */
	void setWeight( const csl::Pattern& pattern, float weight );



	float getDefault( std::pair< size_t, size_t > const& patternType ) const;   


	void setDefault( std::pair< size_t, size_t > const& patternType, float weight );

	/**
	 * @brief prints the pattern weights
	 */
	void printPatternWeights() const;
	

	private:
	typedef std::pair< size_t, size_t > PatternType;

	/**
	 * @brief This data structure maps the known patterns to their weights
	 */
	std::map< csl::Pattern, float> patternWeights_;

	/**
	 *
	 */
	std::map< PatternType, float > defaultWeights_;


   }; // class PatternWeights



} // ns csl

#endif
