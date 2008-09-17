#ifndef CSL_VAM_H
#define CSL_VAM_H CSL_VAM_H

#include<iostream>

#include<vector>
#include<stack>
#include<set>

#include "../Global.h"
#include "../Stopwatch.h"
#include "../MinDic/MinDic.h"
#include "../LevDEA/LevDEA.h"

#include "./PatternGraph.h"
#include "../Pattern/Interpretation.h"


namespace csl {

    /*
     * Vam - Variant-Aware Matching
     */

    /**
     * @brief Vam is a module for matching in large dictionaries, considering
     * spelling variants (defined with a set of rewrite patterns).
     *
     * As input the tool needs 
     * - a wordList @c baseDic compiled as FSA (use the tool compileMD to get such an automaton from a word list. 
     * - a file containing a list of patterns, one per line. A line containing 't th' indicates that a 't' in 
     *   @c baseDic could be transformed to 'th'.
     * - a threshold @c maxNrOfPatterns specifying the maximum number of pattern applications in one word (default: 1000, which is near to infinite ).
     *
     * Definition of the output:
     *
     * We define a so-called hypothetical dictionary @c hypotheticalDic that contains all spelling variants
     * which can be obtained by applying a sequence of up to @c maxNrOfPatterns variant patterns to any 
     * word of @c baseDic. One entry of @c hypotheticalDic is a triple @c{<word,baseWord,instruction>} where
     * @c instruction is the above mentioned sequence to get the variant @c word from the original word 
     * @c baseWord from @c baseDic.
     * 
     * For a query @c q the algorithm returns all such triples @c{<word,baseWord,instruction>} from @c hypotheticalDic
     * where @c word equals @c q
     *
     * @see csl::Interpretation, csl::Instruction, csl::PosPattern, csl::Pattern
     *
     * @author Ulrich Reffle, 2008
     */
    class Vam {

    public:
	/**
	 * @brief The type of MinDic used as dictionaries
	 */
	typedef MinDic<> MinDic_t;

    private:

	/**
	 * A state of the MinDic
	 */
	typedef MinDic_t::State MDState_t;

    public:
	/**
	 * @brief A trivial construtor, taking the input ressources as arguments
	 *
	 * @param baseDic a reference to a MinDic_t that serves as @c baseDic
	 * @param patternFile path to a file containing the spelling variant patterns 
	          (see class description for some more details). 
	 */
	Vam( const MinDic_t& baseDic, const char* patternFile );

	/**
	 * @name Configuration
	 */
	//@{

	/**
	 * @brief In addition to pattern applications, allow fuzzy search with distance up to @c d
	 */
	inline void setDistance( size_t d );

	/**
	 * @brief restrict allowed number of pattern applications to greater or equal than @c n
	 */
	inline void setMinNrOfPatterns( size_t n );

	/**
	 * @brief restrict allowed number of pattern applications to less or equal than @c n
	 */
	inline void setMaxNrOfPatterns( size_t n );

	/**
	 * @brief set a filter dictionary. This restricts output words to words NOT present in this filterDic.
	 */
	inline void setFilterDic( MinDic_t const& filterDic );
	
	// @}

	/**
	 * @name Usage/ Queries
	 */
	//@{
	/**
	 * @brief query a @c word to get possible interpretations as a variant.
	 *
	 * @todo Passing a std::vector* as answer container is probably not very elegant.
	 */
	inline bool query( std::wstring const& word, std::vector< Interpretation >* interpretations ) const;

	//@}

    private:
	inline void query_rec( size_t depth ) const;


	class Position {
	public:
	    Position( size_t wordPos, size_t nrOfPatternsApplied, const std::pair< int, int >& mother = std::make_pair( -1, -1 ) ) :
		wordPos_( wordPos ),
		posPattern_(),
		nrOfPatternsApplied_( nrOfPatternsApplied ) {
		mother_ = mother;
	    }
	    
	    void addPosPattern( const PosPattern& posPattern ) {
		posPattern_ = posPattern;
	    }

	    size_t getNrOfPatternsApplied() const {
		return nrOfPatternsApplied_;
	    }

	    size_t wordPos_;
	    PosPattern posPattern_;

	    size_t nrOfPatternsApplied_;

	    /**
	     * (x,y) indicates that the mother is the y-th element at stackpos x (if I remember correctly)
	     */ 
	    std::pair< int, int > mother_;
	}; // class Position

	class StackItem : public std::vector< Position > {
	public:
	    StackItem( const Vam& myVam ) :
		dicPos_( myVam.baseDic_ ),
		patternPos_( myVam.patternGraph_, 0 ),
		lookAheadDepth_( 0 ) {
	    }

	    void clear() {
		std::vector< Position >::clear();
		// somehow reset dicPos_???
	    }

	    MDState_t dicPos_;
	    PatternGraph::State patternPos_;
	    size_t lookAheadDepth_;
	};
	
	class Stack : public std::vector< StackItem > {
	public:
	    Stack() {
		// reserve( 500 );
	    }
	};
	
	void printPosition( const Position& pos ) const;

	/**
	 */
	void reportMatch( const Position* cur, int baseWordScore ) const;

	/**
	 * This method picks up all patterns used to get the match and adds them
	 * to the instruction of the \c Interpretation -object
	 *
	 * @param[in]  cur
	 * @param[in]  baseWordScore
	 * @param[out] answer
	 */
	void reportMatch_rec( const Position* cur, Interpretation* answer ) const;

	/////   DATA MEMBERS OF VAM    //////////////////////

	MinDic_t const& baseDic_;
	MinDic_t const* filterDic_;

	PatternGraph patternGraph_;
	std::vector< std::wstring > leftSidesList_;
	std::vector< PatternGraph::RightSides_t > rightSides_;
	
	mutable LevDEA levDEA_;
	mutable std::wstring query_;
	mutable std::vector< Interpretation >* interpretations_;
	mutable Stack stack_;
	/**
	 * The current string under construction
	 */
	mutable std::wstring baseWord_;

	size_t minNrOfPatterns_;
	size_t maxNrOfPatterns_;
	
    }; // class Vam
    
} // namespace csl

#include "./Vam.tcc"

#endif
