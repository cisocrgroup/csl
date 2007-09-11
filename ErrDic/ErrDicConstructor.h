#include<iostream>

#include<vector>
#include<stack>
#include<set>


#include "../Global.h"
#include "../MinDic/MinDic.h"

namespace csl {

    class PatternApplier {
    private:

	/**
	 * This class represents a position in the dictionary automaton. One or more of those objects 
	 * form one stack item for the unified traversal of different branches of the automaton.
	 */
	class Position {
	public:
	    inline Position();
		
	    inline Position( StateId_t state, const wchar_t* nextChar, bool hasError );

	    inline bool operator<( const Position& other ) const;

	    inline void set(  StateId_t state, const wchar_t* curChar, bool hasError );

	    inline const StateId_t& getState() const;

	    inline size_t hasError() const;

	    inline wchar_t getNextChar() const;

	    inline bool increaseNextChar();
	    
	    inline const Position* getBackPtr() const;

	private:
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    bool hasError_;
	}; // class Position


	/**
	 * Objects of this class are used to keep track of when the error pattern can be applied.
	 */
	class PatternTracer  {
	public:
	    PatternTracer( StateId_t state, const wchar_t* patternPos ) :
		state_( state ),
		patternPos_( patternPos ) {
	    }

	    inline bool operator<( const PatternTracer& other ) const {
		return *patternPos_ < *( other.patternPos_ );
	    }
    
	    inline wchar_t getNextChar() const {
		return *patternPos_;
	    }

	    StateId_t state_;
	    const wchar_t* patternPos_;
	};

	class StackItem {
	public:
	    typedef std::vector< Position > PositionContainer;
	    typedef std::vector< PatternTracer > PatTracerContainer;
	    
	    StackItem() :
		filterPos_( 0 ) {
	    }

	    void clear() {
		positions_.clear();
		patternTracers_.clear();
		filterPos_ = 0;
	    }

	    PositionContainer positions_;
	    PatTracerContainer patternTracers_;
	    StateId_t filterPos_;

	};

	class Stack : public std::vector< StackItem > {
	public:
	    Stack() :
		std::vector< StackItem >( 2 ),
		depth_( 0 ) {
	    }
	    
	    size_t getDepth() const {
		return depth_;
	    }


	    void forward() {
		depth_ += 1;
		if( size() <= depth_ + 1 ) resize( depth_ + 2 );
		// std::cerr<<"forward: depth is "<<getCurDepth()<<std::endl;
	    }
	    
	    bool back() {
		if( depth_ == 0 ) return false;
		at( depth_ ).clear();
		at( depth_ + 1 ).clear(); // this might be in vain, as it should be empty anyway ?!
		
		--depth_;
		word_.resize( depth_ );
		// std::cerr<<"back: depth is "<<getCurDepth()<<std::endl;
		return true;
	    }

	    std::wstring& getWord() {
		return word_;
	    }

	    const std::wstring& getConstWord() const {
		return word_;
	    }

	private:
	    size_t depth_;
	    std::wstring word_;
	};


    public:
	PatternApplier( const MinDic< int >& dic, const MinDic< int >& filterDic, const std::wstring& patternFrom, const std::wstring& patternTo ) :
	    dic_( dic ),
	    filterDic_( filterDic ),
	    stack_(),
	    patternFrom_( patternFrom ),
	    patternTo_( patternTo ) {


	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    stack_.at( 0 ).positions_.push_back( Position( st, suso, false ) );
	    if( size_t newState = dic_.walkStr( st, patternFrom_.c_str() ) ) {
		stack_.at( 0 ).patternTracers_.push_back( PatternTracer( newState, patternTo_.c_str() ) );
	    }

	}

	const std::wstring& getWord() const {
	    return ( stack_.getConstWord() );
	}

	bool next() {
	    bool foundFinal = false;
	    bool stackNotEmpty = true;
	    wchar_t label = 0;

	    bool foundContinuation = false;
	    do { // this loop terminates at a final state
		// std::cerr<<"Enter final loop"<<std::endl;
		do { // this loop terminates if one continuation was found or if the stackItem is done
		    // std::cerr<<"Enter conti loop"<<std::endl;


		    std::sort( stack_.at( getCurDepth() ).positions_.begin(), stack_.at( getCurDepth() ).positions_.end() );
		    std::sort( stack_.at( getCurDepth() ).patternTracers_.begin(), stack_.at( getCurDepth() ).patternTracers_.begin() );

		    StackItem::PositionContainer::iterator positionIt = stack_.at( getCurDepth() ).positions_.begin();
		    StackItem::PatTracerContainer::iterator patTracerIt = stack_.at( getCurDepth() ).patternTracers_.begin();

		    foundContinuation = false;

		    if( positionIt->getNextChar() < patTracerIt->getNextChar() ) {

			label = positionIt->getNextChar();

			// go through all Positions with that label
			while( ( positionIt != stack_.at( getCurDepth() ).positions_.end() ) && 
			       ( positionIt->getNextChar() == label ) ) {

			    checkPosition( *positionIt, foundContinuation, foundFinal );
			    ++positionIt;
			} // for all Positions with the same label
		    } // if posIt < tracerIt


		    /////////////    POSITION == TRACER //////////////////////
 		    else if( positionIt->getNextChar() == patTracerIt->getNextChar() ) {
 			label = positionIt->getNextChar();
			
			// go through all Positions with that label
			while( ( positionIt != stack_.at( getCurDepth() ).positions_.end() ) && 
			       ( positionIt->getNextChar() == label ) ) {
			    checkPosition( *positionIt, foundContinuation, foundFinal );
			    ++positionIt;
			} // for all Positions with the same label

		    }

		} while( ( ! foundContinuation ) && label != 0 );
		if( label == 0 ) {
		    stackNotEmpty = stack_.back();
		}
		else {
		    stack_.forward();
		}
		
	    } while( !foundFinal && stackNotEmpty );
	    return ( stackNotEmpty );
	}
	
    private:
	void checkPosition( Position& pos, bool& foundContinuation, bool& foundFinal ) {
	    wchar_t label = pos.getNextChar();
	    StateId_t nextState = dic_.walk( pos.getState(), label );
	    if( nextState ) {
		addContinuation( nextState, dic_.getSusoString( nextState ), pos.hasError() );
		if( size_t newState = dic_.walkStr( nextState, patternFrom_.c_str() ) ) {
		    addPatternTracer( newState, patternTo_.c_str() );
		}
				
		if( ! foundContinuation ) { // if continuation wasn't registered before
		    foundContinuation = true;
		    stack_.getWord().resize( getCurDepth() + 1 );
		    stack_.getWord().at( getCurDepth() ) = label;
				
				    
		    stack_.at( getCurDepth() + 1 ).filterPos_ = filterDic_.walk( stack_.at( getCurDepth() ).filterPos_, label );

		    std::wcout<<stack_.getWord()<<std::endl;
		} // register continuation for first time
			    
		foundFinal = 
		    pos.hasError() &&
		    dic_.isFinal( nextState ) && 
		    ! filterDic_.isFinal( stack_.at( getCurDepth() + 1 ).filterPos_ );
			    
		pos.increaseNextChar();
				
				
	    } // if nextState
	    else std::cerr<<"VERY STRANGE"<<std::endl;
	}

	void addContinuation( StateId_t state, const wchar_t* susoStr, bool hasError ) {
	    stack_.at( getCurDepth() +1 ).positions_.push_back( Position( state, susoStr, hasError ) );
	}
	
	void addPatternTracer( StateId_t state, const wchar_t* patternPos ) {
	    stack_.at( getCurDepth() + 1 ).patternTracers_.push_back( PatternTracer( state, patternPos ) );
	}

	    

	Stack stack_;

	size_t getCurDepth() const {
	    return stack_.getDepth();
	}

	const MinDic< int >& dic_;
	const MinDic< int >& filterDic_;

	// the error pattern
	std::wstring patternFrom_;
	std::wstring patternTo_;

    }; // class PatternApplier




} // eon

#include "./Position.tcc"

    
