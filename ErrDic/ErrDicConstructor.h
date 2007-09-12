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

	    inline bool stepToNextChar();
	    
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
	    PatternTracer( size_t startStackPos, const wchar_t* patternPos ) :
		startStackPos_( startStackPos ),
		patternPos_( patternPos ) {
	    }

	    inline bool operator<( const PatternTracer& other ) const {
		return wcscmp( patternPos_, other.patternPos_ ) < 0;
	    }
    
	    inline wchar_t getNextChar() const {
		return *patternPos_;
	    }

	    inline bool stepToNextChar() {
		if( *patternPos_ == 0 ) return false;
		++patternPos_;
		return true;
	    }

 	    inline const wchar_t* getPatternPos() const {
		return patternPos_;
	    }

 	    inline const size_t getStartStackPos() const {
		return startStackPos_;
	    }

	    size_t startStackPos_;
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

	    void sort() {
		std::sort( positions_.begin(), positions_.end() );
		std::sort( patternTracers_.begin(), patternTracers_.end() );

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

	    StackItem::PositionContainer& getPositions() {
		return at( depth_ ).positions_;
	    }

	    StackItem::PatTracerContainer& getPatTracers() {
		return at( depth_ ).patternTracers_;
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

	    stack_.at( 0 ).patternTracers_.push_back( PatternTracer( 0, patternTo_.c_str() ) );

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


		    stack_.at( getCurDepth() ).sort();

		    std::wcout<<":"<<stack_.getConstWord()<<std::endl;
		    StackItem::PositionContainer::iterator positionIt = stack_.getPositions().begin();
		    while( positionIt != stack_.getPositions().end() && positionIt->getNextChar() == 0 ){// skip worn-out positions

			/// CONTINUE TO THINK HERE !!!


//			++positionIt;
//			positionIt = stack_.getPositions().erase( positionIt );
		    }

		    StackItem::PatTracerContainer::iterator patTracerIt = stack_.getPatTracers().begin();

		    foundContinuation = false;

		    bool positionsEmpty = ( positionIt == stack_.getPositions().end() );
		    bool patTracersEmpty = ( patTracerIt == stack_.getPatTracers().end() );

		    if( positionsEmpty && patTracersEmpty ) {
			// nothing to do here
		    }
		    ///// patTracer is either empty or greater than the position
		    else if( ! positionsEmpty && ( patTracersEmpty || ( positionIt->getNextChar() < patTracerIt->getNextChar() ) ) ) {
			
			label = positionIt->getNextChar();
			
			// go through all Positions with that label
			while( ( positionIt != stack_.getPositions().end() ) && 
			       ( positionIt->getNextChar() == label ) ) {
			    
			    checkPosition( *positionIt, foundContinuation, foundFinal );
			    ++positionIt;

			} // for all Positions with the same label
		    } // if posIt < tracerIt


		    /////////////    POSITION == TRACER //////////////////////
		    else if( !positionsEmpty && !patTracersEmpty && ( positionIt->getNextChar() == patTracerIt->getNextChar() ) ) {
			label = positionIt->getNextChar();
			
			// go through all Positions with that label
			while( ( positionIt != stack_.getPositions().end() ) && 
			       ( positionIt->getNextChar() == label ) ) {
			    checkPosition( *positionIt, foundContinuation, foundFinal );
			    ++positionIt;
			} // for all Positions with the same label
			
			// go through all PatternTracers with that label
			while( ( patTracerIt != stack_.getPatTracers().end() ) && 
			       ( patTracerIt->getNextChar() == label ) ) {

			    patTracerIt->stepToNextChar();
			    if( patTracerIt->getNextChar() == 0 ) {
				for( 
				    StackItem::PositionContainer::iterator backIt = stack_.getPositions().begin();
				    backIt != stack_.getPositions().end();
				    ++backIt
				    ) {
				    if( StateId_t newState = dic_.walkStr( backIt->getState(), patternFrom_.c_str() ) ) {
					addContinuation( newState, dic_.getSusoString( newState), 1 );
				    }
				}
			    }
			    else {
				addPatternTracer( patTracerIt->getStartStackPos(), patTracerIt->getPatternPos() );
			    }

			    stack_.getWord().resize( getCurDepth() + 1 );
			    stack_.getWord().at( getCurDepth() ) = label;

			    patTracerIt = stack_.getPatTracers().erase( patTracerIt );
			    foundContinuation = true;
			} // for all PatternTracers with the same label
			
		    } // POSITION == TRACER

		    // TRACER < POSITION
		    else if( ! patTracersEmpty && ( positionsEmpty || ( patTracerIt->getNextChar() < positionIt->getNextChar() ) ) ) {

			label = patTracerIt->getNextChar();
			// go through all PatternTracers with that label
			while( ( patTracerIt != stack_.getPatTracers().end() ) && 
			       ( patTracerIt->getNextChar() == label ) ) {

			    patTracerIt->stepToNextChar();
			    if( patTracerIt->getNextChar() == 0 ) {
				for( 
				    StackItem::PositionContainer::iterator backIt = stack_.at( patTracerIt->getStartStackPos() ).positions_.begin();
				    backIt != stack_.getPositions().end();
				    ++backIt
				    ) {
				    if( StateId_t newState = dic_.walkStr( backIt->getState(), patternFrom_.c_str() ) ) {
					addContinuation( newState, dic_.getSusoString( newState), 1 );
				    }
				}
			    }
			    else {
				addPatternTracer( patTracerIt->getStartStackPos(), patTracerIt->getPatternPos() );
			    }

			    stack_.getWord().resize( getCurDepth() + 1 );
			    stack_.getWord().at( getCurDepth() ) = label;

			    patTracerIt = stack_.getPatTracers().erase( patTracerIt );
			    foundContinuation = true;
			} // for all PatternTracers with the same label
			
		    } // TRACER < POSITION
			     

		} while( ( ! foundContinuation ) &&  ( stack_.getPositions().size() || stack_.getPatTracers().size() ) );

		if( stack_.at( getCurDepth() + 1 ).positions_.size() == 0 &&
		    stack_.at( getCurDepth() + 1 ).patternTracers_.size() == 0 ) {
		    stackNotEmpty = stack_.back();
		}
		else {
		    if( stack_.getPositions().size() > 0 ) addPatternTracer( getCurDepth() + 1, patternTo_.c_str() );
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
			    
		bool ret = pos.stepToNextChar(); // return value is for DEBUG
		assert( ret ); // DEBUG
				
				
	    } // if nextState
	    else std::cerr<<"VERY STRANGE"<<std::endl;
	}

	void addContinuation( StateId_t state, const wchar_t* susoStr, bool hasError ) {
	    stack_.at( getCurDepth() +1 ).positions_.push_back( Position( state, susoStr, hasError ) );
	}
	
	void addPatternTracer( size_t startStackPos, const wchar_t* patternPos ) {
	    stack_.at( getCurDepth() + 1 ).patternTracers_.push_back( PatternTracer( startStackPos, patternPos ) );
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

    
    
