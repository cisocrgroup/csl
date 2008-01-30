#include<iostream>

#include<vector>
#include<stack>
#include<set>

#include "../Global.h"
#include "../Stopwatch.h"
#include "../MinDic/MinDic.h"
#include "../LevDEA/LevDEA.h"


namespace csl {

    class Vam {

    private:
	typedef MinDic<> MinDic_t;

	/**
	 * A state of the MinDic
	 */
	typedef MinDic_t::State MDState_t;

	/**
	 * For each left pattern side, this container type holds the appropriate list
	 * of right pattern sides
	 */
	typedef std::vector< std::wstring > RightList_t;


    public:
	struct Answer {
	    std::wstring word;
	    std::vector< std::pair< std::wstring, std::wstring > > patterns;
	};

	Vam( const MinDic_t& basedDic, const char* patternFile );

	inline void setDistance( size_t d );

	inline void query( const std::wstring& word, std::vector< Answer >* answers );

	inline void query_rec( size_t depth );
	

    private:

	/**
	 * @brief Error is usually stored with a Position and represents one error operation 
	 * that was used during the long life of the Position
	 *
	 */
	class Error {
	public:
	    Error();
	    
	    Error( const std::wstring& pattern, size_t position ) :
		pattern_( pattern ),
		position_( position ) {
		
	    }
	    
	    const std::wstring& getPattern() const;
	    size_t getPosition() const;
	    
	    
	private:
	    std::wstring pattern_;
	    /**
	     * The char offset in the word where the error was inserted
	     */
	    int position_;
	};
	

	class Position {
	public:
	    Position( const LevDEA::Pos& levPos, const Position* mother = 0 ) :
		levPos_( levPos ),
		error_( 0 ),
		mother_( mother )
		{
	    }
	    ~Position() {
		if( error_ ) delete( error_ );
	    }

	    /**
	     * The user has to create the error on the heap, this class
	     * does the destruction
	     */
	    void addError( Error* error ) {
		error_ = error;
	    }

	    LevDEA::Pos levPos_;
	    Error* error_;
	    const Position* mother_;
	};

	class StackItem : public std::vector< Position > {
	public:
	    StackItem( const Vam& myVam ) :
		dicPos_( myVam.baseDic_ ) {
	    }

	    void clear() {
		std::vector< Position >::clear();
		// somehow reset dicPos_???
	    }

	    MDState_t dicPos_;
	};
	
	class Stack : public std::vector< StackItem > {
	    
	};
	
	void loadPatterns( const char* patternFile );


	/////   DATA MEMBERS OF VAM   //////////////////////

	const MinDic_t& baseDic_;
	LevDEA levDEA_;

	MinDic_t leftSides_;
	std::vector< std::wstring > leftSidesList_;
	std::vector< RightList_t > rightSides_;

	std::wstring query_;
	std::vector< Answer >* answers_;

	Stack stack_;
	/**
	 * The current string under construction
	 */
	std::wstring word_;
	
	
    }; // class Vam
    
} // namespace csl


#include "./Vam.tcc"
