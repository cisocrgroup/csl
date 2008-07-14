
#include<iostream>

#include<vector>
#include<stack>
#include<set>

#include "../Global.h"
#include "../Stopwatch.h"
#include "../VariantRecognizer/VariantRecognizer.h"
#include "../MinDic/MinDic.h"
#include "../LevDEA/LevDEA.h"

#include "./PatternGraph.h"


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
	Vaam( const MinDic_t& basedDic, const char* patternFile );

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
	inline void query( const std::wstring& word, std::vector< Answer >* answers );

	

    private:
	inline void query_rec( size_t depth );


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
	void reportMatch( const Position* cur ) const;

	/**
	 * This method picks up all patterns used to get the match and adds them
	 * to the instruction of the \c Answer -object
	 */
	void reportMatch_rec( const Position* cur, Answer* answer ) const;

	/////   DATA MEMBERS OF VAM   //////////////////////

	const MinDic_t& baseDic_;
	LevDEA levDEA_;

	PatternGraph patternGraph_;
	std::vector< std::wstring > leftSidesList_;
	std::vector< PatternGraph::RightSides_t > rightSides_;
	
	std::wstring query_;
	std::vector< Answer >* answers_;

	Stack stack_;
	/**
	 * The current string under construction
	 */
	std::wstring word_;

	size_t maxNrOfPatterns_;
	
    }; // class Vaam
    
} // namespace csl

#include "./Vaam.tcc"
