
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

    /**
     * Vaam - Variant-aware Approximate Matching
     *
     * @author Ulrich Reffle, 2008
     */
    class Vaam {

    private:
	typedef MinDic<> MinDic_t;

	/**
	 * A state of the MinDic
	 */
	typedef MinDic_t::State MDState_t;

    public:
	Vaam( const MinDic_t& baseDic, const char* patternFile );

	/**
	 * @brief In addition to pattern applications, allow fuzzy search with distance up to @c d
	 */
	inline void setDistance( size_t d );

	/**
	 * @brief restrict allowed number of pattern applications to less or equal than @c n
	 */
	inline void setMaxNrOfPatterns( size_t n );

	/**
	 * @brief query a @c word to get possible interpretations as a variant.
	 */
	inline bool query( std::wstring const& word, std::vector< Interpretation >* interpretations ) const;

	

    private:
	inline void query_rec( size_t depth ) const;


	class Position {
	public:
	    Position( const LevDEA::Pos& levPos, size_t nrOfPatternsApplied, const std::pair< int, int >& mother = std::make_pair( -1, -1 ) ) :
		levPos_( levPos ),
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

	    LevDEA::Pos levPos_;
	    PosPattern posPattern_;

	    size_t nrOfPatternsApplied_;

	    /**
	     * (x,y) indicates that the mother is the y-th element at stackpos x (if I remember correctly)
	     */ 
	    std::pair< int, int > mother_;
	};

	class StackItem : public std::vector< Position > {
	public:
	    StackItem( const Vaam& myVaam ) :
		dicPos_( myVaam.baseDic_ ),
		patternPos_( myVaam.patternGraph_, 0 ),
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

	/////   DATA MEMBERS OF VAAM    //////////////////////

	const MinDic_t& baseDic_;

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
	mutable std::wstring word_;

	size_t maxNrOfPatterns_;
	
    }; // class Vaam
    
} // namespace csl

#include "./Vaam.tcc"
