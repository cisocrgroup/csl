#include<iostream>

#include<vector>
#include<stack>
#include<set>


#include "../Global.h"
#include "../MinDic/MinDic.h"

namespace csl {

    class PatternApplier {
    private:

	class ListItem {
	public:
	    virtual ~ListItem();
	    
	    inline void setNextItem( ListItem* nextItem ) {
		nextItem_ = nextItem;
	    }
	    inline ListItem* getNextItem() const {
		return next_;
	    }

	    virtual wchar_t getNextChar() const;
	    virtual bool stepToNextChar();

	    virtual bool hasPathWithPattern() const;
	    
	private:
	    ListItem* nextItem_;
	};
	/**
	 * This class represents a position in the dictionary automaton. One or more of those objects 
	 * form one stack item for the unified traversal of different branches of the automaton.
	 */
	class Position : public ListItem {
	public:
	    inline Position() {
	    } 
		
	    inline Position( StateId_t state, const wchar_t* nextChar, bool hasError );

	    inline bool operator<( const Position& other ) const;

	    inline void set(  StateId_t state, const wchar_t* curChar, bool hasError );

	    inline const StateId_t& getState() const;

	    inline size_t hasError() const;

	    inline wchar_t getNextChar() const;

	    inline bool stepToNextChar();
	    

	private:
	    StateId_t state_;
	    const MinDic< int >& dic_;
	    const wchar_t* nextChar_;
	    bool hasError_;
	}; // class Position


	/**
	 * Objects of this class are used to keep track of when the error pattern can be applied.
	 */
	class PatternTracer : public ListItem {
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

	    inline bool hasPathWithPattern() const {
		return false;
	    }

	    size_t startStackPos_;
	    const wchar_t* patternPos_;
	};

	class StackItem {
	public:
	    
	    StackItem() :
		list_( 0 ),
		filterPos_( 0 ) {
	    }

	    ~StackItem() {
		clear();
	    }

	    void clear() {
		ListItem* listPos = list_;
		ListItem* nextPos = 0;
		while( ( listPos != 0 ) ) {
		    nextPos = listPos->getNextItem();
		    delete( listPos );
		    listPos = nextPos;
		}
		
		filterPos_ = 0;
		newStates_.clear();
	    }

	    void addItem( ListItem* newItem ) {

		ListItem* listPos = list_;
		ListItem* lastPos = 0;
		while( ( listPos != 0 ) && listPos->getNextChar() < newItem->getNextChar() ) {
		    lastPos = listPos;
		    listPos = listPos->getNextItem();
		    
		}
		if( lastPos ) lastPos->setNextItem( newItem );
		newItem->setNextItem( listPos );
	    }

	    void sortItem( ListItem* item ) {
	    }

	    void addNewState( StateId_t state ) {
		newStates_.push_back( state );
	    }

	    void splice( ) {
	    }

	private:
	    std::list< ListItem* > list_;
	    StateId_t filterPos_;
	    std::vector< StateId_t > newStates_;

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

	    const std::wstring& getWord() const {
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
	    patternFrom_( patternFrom ),
	    patternTo_( patternTo ),
	    isGood_( false ){
	    
	    
	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    stack_.at( 0 ).positions_.push_back( Position( st, suso, false ) );

	    stack_.at( 0 ).patternTracers_.push_back( PatternTracer( 0, patternTo_.c_str() ) );

	    tokenCount_ = 0;
	    next();

	    // std::wcerr<<"New PatternApplier with: "<<patternFrom_<<"->"<<patternTo_<<std::endl;
	}

	void printMe() {
	    std::wcout<<"pa with pattern "<<patternFrom_<<"->"<<patternTo_<<" have spit out "<<tokenCount_<<" tokens"<<std::endl; 
	}

	bool operator<( const PatternApplier& other ) const {
	    return getWord() < other.getWord();
	}
	
	const std::wstring& getWord() const {
	    return ( stack_.getWord() );
	}

	const std::wstring& getPatternFrom() const {
	    return patternFrom_;
	}

	const std::wstring& getPatternTo() const {
	    return patternTo_;
	}

	bool isGood() const {
	    return isGood_;
	}

	bool next() {
	    isGood_ = true;
	    bool foundFinal = false;
	    wchar_t label = 0;

	    bool foundContinuation = false;
	    do { // this loop terminates at a final state
		// std::cerr<<"Enter final loop"<<std::endl;

		bool positionsEmpty;
		bool patTracersEmpty;
		do { // this loop terminates if one continuation was found or if the stackItem is done
		    // std::cerr<<"Enter conti loop"<<std::endl;


		    StackItem& stackItem = stack_.at( getCurDepth() );
		    
		    foundContinuation = false;


		    if( positionsEmpty && patTracersEmpty ) {
			// nothing to do here
		    }
		    ///// patTracer is either empty or greater than the position
		    else if( ! positionsEmpty && ( patTracersEmpty || ( positionIt->getNextChar() < patTracerIt->getNextChar() ) ) ) {
			
			label = positionIt->getNextChar();
			
			// go through all Positions with that label
			while( ( positionIt != positions.end() ) && 
			       ( positionIt->getNextChar() == label ) ) {
			    
			    checkPosition( *positionIt, foundContinuation, foundFinal );
			    ++positionIt;

			} // for all Positions with the same label
		    } // if posIt < tracerIt


		    /////////////    POSITION == TRACER //////////////////////
		    else if( !positionsEmpty && !patTracersEmpty && ( positionIt->getNextChar() == patTracerIt->getNextChar() ) ) {
			label = positionIt->getNextChar();
			
			// go through all Positions with that label
			while( ( positionIt != positions.end() ) && 
			       ( positionIt->getNextChar() == label ) ) {
			    checkPosition( *positionIt, foundContinuation, foundFinal );
			    ++positionIt;
			} // for all Positions with the same label
			
			// go through all PatternTracers with that label
			while( ( patTracerIt != patTracers.end() ) && 
			       ( patTracerIt->getNextChar() == label ) ) {

			    patTracerIt->stepToNextChar();
			    if( patTracerIt->getNextChar() == 0 ) {
				// std::cerr<<"depth is "<<getCurDepth()<<"Consider newStates at depth " << patTracerIt->getStartStackPos() << std::endl;
				for( 
				    StackItem::PositionContainer::iterator backIt = stack_.at( patTracerIt->getStartStackPos() ).positions_.begin();
				    backIt != stack_.at( patTracerIt->getStartStackPos() ).positions_.end();
				    ++backIt
				    ) {
				    
				    // std::cerr<<"Try backstate="<<backIt->getState()<<std::endl;
				    StateId_t newState;
				    if( ( ! backIt->hasError() ) && ( newState = dic_.walkStr( backIt->getState(), patternFrom_.c_str() ) ) ) {
					if( dic_.isFinal( newState ) ) foundFinal = true;
					addContinuation( newState, dic_.getSusoString( newState), 1 );
					foundContinuation = true;
				    }
				}
			    }
			    else {
				addPatternTracer( patTracerIt->getStartStackPos(), patTracerIt->getPatternPos() );
				foundContinuation = true;
			    }
			    
			    if( foundContinuation ) {
				stack_.getWord().resize( getCurDepth() + 1 );
				stack_.getWord().at( getCurDepth() ) = label;
			    }

			    patTracerIt = patTracers.erase( patTracerIt );
			} // for all PatternTracers with the same label
			
		    } // POSITION == TRACER

		    // TRACER < POSITION
		    else if( ! patTracersEmpty && ( positionsEmpty || ( patTracerIt->getNextChar() < positionIt->getNextChar() ) ) ) {

			label = patTracerIt->getNextChar();
			// go through all PatternTracers with that label
			while( ( patTracerIt != patTracers.end() ) && 
			       ( patTracerIt->getNextChar() == label ) ) {

			    patTracerIt->stepToNextChar();
			    if( patTracerIt->getNextChar() == 0 ) {
				// std::cerr<<"depth is "<<getCurDepth()<<"Consider newStates at depth " << patTracerIt->getStartStackPos() << std::endl;
				for( 
				    StackItem::PositionContainer::iterator backIt = stack_.at( patTracerIt->getStartStackPos() ).positions_.begin();
				    backIt != stack_.at( patTracerIt->getStartStackPos() ).positions_.end();
				    ++backIt
				    ) {
				    
				    // std::cerr<<"Try backstate="<<backIt->getState()<<std::endl;
				    StateId_t newState;
				    if( ( ! backIt->hasError() ) && ( newState = dic_.walkStr( backIt->getState(), patternFrom_.c_str() ) ) ) {
					if( dic_.isFinal( newState ) ) foundFinal = true;
					addContinuation( newState, dic_.getSusoString( newState), 1 );
					foundContinuation = true;
				    }
				}
			    }
			    else {
				addPatternTracer( patTracerIt->getStartStackPos(), patTracerIt->getPatternPos() );
				foundContinuation = true;
			    }
			    
			    if( foundContinuation ) {
				stack_.getWord().resize( getCurDepth() + 1 );
				stack_.getWord().at( getCurDepth() ) = label;
			    }

			    patTracerIt = patTracers.erase( patTracerIt );
			} // for all PatternTracers with the same label
			
		    } // TRACER < POSITION
			     

		} while( ( ! foundContinuation ) &&  ( !positionsEmpty || !patTracersEmpty ) );
		
		if( stack_.at( getCurDepth() + 1 ).positions_.size() == 0 &&
		    stack_.at( getCurDepth() + 1 ).patternTracers_.size() == 0 ) {

		    isGood_ = stack_.back();
		}
		else {
		    if( stack_.at( getCurDepth() + 1 ).positions_.size() > 0 )
			addPatternTracer( getCurDepth() + 1, patternTo_.c_str() );
		    stack_.forward();
		}
		
	    } while( !foundFinal && isGood() );
	    
	    if( isGood() ) ++tokenCount_;

	    return isGood();
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

		    // std::wcout<<stack_.getWord()<<std::endl;
		} // register continuation for first time
			    
		if( pos.hasError() &&
		    dic_.isFinal( nextState ) && 
		    ! filterDic_.isFinal( stack_.at( getCurDepth() + 1 ).filterPos_ ) ) {
		    foundFinal = true;
		}
			    
		pos.stepToNextChar();
				
				
	    } // if nextState
	    else std::cerr<<"VERY STRANGE"<<std::endl;
	}

	void addContinuation( StateId_t state, const wchar_t* susoStr, bool hasError ) {
	    stack_.at( getCurDepth() +1 ).positions_.push_back( Position( state, susoStr, hasError ) );
	}
	
	void addPatternTracer( size_t startStackPos, const wchar_t* patternPos ) {
	    stack_.at( getCurDepth() + 1 ).patternTracers_.push_back( PatternTracer( startStackPos, patternPos ) );
	}

	    
	size_t getCurDepth() const {
	    return stack_.getDepth();
	}


	Stack stack_;
	const MinDic< int >& dic_;
	const MinDic< int >& filterDic_;

	// the error pattern
	std::wstring patternFrom_;
	std::wstring patternTo_;

	size_t tokenCount_;
	bool isGood_;

    }; // class PatternApplier




} // eon

#include "./Position.tcc"

    
    
