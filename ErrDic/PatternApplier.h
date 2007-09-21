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
	    inline ListItem() : 
		nextItem_( 0 ) {
	    }

	    inline virtual ~ListItem() {
	    }
	    
	    virtual bool isPosition() const = 0;
	    virtual bool isPatternTracer() const = 0;

	    virtual wchar_t getNextChar() const = 0;
	    virtual bool stepToNextChar() = 0;


	    inline void setNextItem( ListItem* nextItem ) {
		nextItem_ = nextItem;
	    }
	    inline ListItem* getNextItem() const {
		return nextItem_;
	    }

	private:
	    ListItem* nextItem_;
	};
	/**
	 * This class represents a position in the dictionary automaton. One or more of those objects 
	 * form one stack item for the unified traversal of different branches of the automaton.
	 */
	class Position : public ListItem {
	public:
	    inline Position();
		
	    inline Position( StateId_t state, const wchar_t* nextChar, bool hasError );

	    inline virtual ~Position() {}

	    inline bool isPosition() const {
		return true;
	    }

	    inline bool isPatternTracer() const {
		return false;
	    }

	    inline bool operator<( const Position& other ) const;

	    inline void set(  StateId_t state, const wchar_t* curChar, bool hasError );

	    inline const StateId_t& getState() const;

	    inline size_t hasError() const;

	    inline wchar_t getNextChar() const;

	    inline bool stepToNextChar();
	    

	private:
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    bool hasError_;
	}; // class Position


	/**
	 * Objects of this class are used to keep track of when the error pattern can be applied.
	 */
	class PatternTracer : public ListItem {
	public:
	    inline PatternTracer( const wchar_t* patternPos ) :
		patternPos_( patternPos ) {
	    }

	    inline ~PatternTracer() {}

	    inline bool operator<( const PatternTracer& other ) const {
		return wcscmp( patternPos_, other.patternPos_ ) < 0;
	    }
    
	    inline bool isPosition() const {
		return false;
	    }

	    inline bool isPatternTracer() const {
		return true;
	    }

	    inline wchar_t getNextChar() const {
		return ( patternPos_ )? *patternPos_ : 0;
	    }

	    // At the moment the first call of this function makes the tracer invalid
	    inline bool stepToNextChar() {
		patternPos_ = 0;
		return false;
	    }

 	    inline const wchar_t* getPatternPos() const {
		return patternPos_;
	    }

	    const wchar_t* patternPos_;
	};

	class StackItem {
	public:
	    StackItem() :
		list_( 0 ),
		filterPos_( 0 ) {
	    }

	    /**
	     * The destructor does NOT call clear(), that is, does NOT remove the ListItem-objects
	     * from the heap. That way, the StackItems can easily be copied.
	     * Call clear() yourself if you now you don't need the StackItem any more ... 
	     */
	    ~StackItem() {
	    }

	    void clear() {
		ListItem* listPos = list_;
		ListItem* nextPos = 0;
		while( ( listPos != 0 ) ) {
		    nextPos = listPos->getNextItem();
		    delete( listPos );
		    listPos = nextPos;
		}
		list_ = 0;
		
		filterPos_ = 0;
		newStates_.clear();
	    }

	    void addNewState( StateId_t newState ) {
		newStates_.push_back( newState );
	    }

	    const std::vector< StateId_t >& getNewStates() const {
		return newStates_;
	    }

	    void addItem( ListItem* newItem ) {
		ListItem* listPos = list_;
		ListItem* lastPos = 0;
		while( ( listPos != 0 ) && listPos->getNextChar() < newItem->getNextChar() ) {
		    lastPos = listPos;
		    listPos = listPos->getNextItem();
		    
		}
		if( lastPos ) lastPos->setNextItem( newItem );
		else list_ = newItem;
		
		newItem->setNextItem( listPos );
	    }
	    
	    ListItem* getFirst() {
		return list_;
	    }

	    void arrangeFirst() {
		// log out first item and add it again
		ListItem* first = list_;
		list_ = list_->getNextItem();

		if( first->getNextChar() == 0 ) {
		    delete( first );
		}
		else {
		    addItem( first );
		}
	    }

	    void setFilterPos( StateId_t pos ) {
		filterPos_ = pos;
	    }

	    StateId_t getFilterPos() const {
		return filterPos_;
	    }

	    bool empty() const {
		return list_ == 0;
	    }

	private:
	    ListItem* list_;
	    StateId_t filterPos_;
	    std::vector< StateId_t > newStates_;

	};


	class Stack : public std::vector< StackItem > {
	public:
	    Stack() :
		std::vector< StackItem >( 2 ),
		depth_( 0 ) {
		reserve( Global::lengthOfStr ); // don not allow realloc - very ugly !!!
	    }
	    
	    size_t getDepth() const {
		return depth_;
	    }

	    void forward() {
		depth_ += 1;
		if( size() <= depth_ + 1 ) {
		    if( capacity() < ( depth_ ) ) {
			throw exceptions::cslException( "PatternApplier: stack threatens to realloc. I'm out of here ..." );
		    }
		    resize( depth_ + 2 );
		}
		// std::cerr<<"forward: depth is "<<getCurDepth()<<std::endl;
	    }
	    
	    bool back() {
		if( depth_ == 0 ) return false;
		at( depth_ ).clear();
		at( depth_ + 1 ).clear(); // this might be in vain, as it should be empty anyway ?!
		
		--depth_;
		// word_.resize( depth_ );
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

	    // insert initial positions and patTracers
	    stack_.at( 0 ).addItem( new Position( st, suso, false ) );

	    if( StateId_t newState = dic_.walkStr( st, patternFrom_.c_str() ) ) {
		stack_.at( 0 ).addNewState( newState );
	    }

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

	    do { // this loop terminates at a final state
		// std::cerr<<"Enter final loop"<<std::endl;


		while( isGood() && stack_.at( getCurDepth() ).empty() ) {
		    isGood_ = stack_.back();
		}

		if( ! isGood_ ) return false;

		StackItem& curStackItem = stack_.at( getCurDepth() );
		StackItem& nextStackItem = stack_.at( getCurDepth() + 1 );
		    
		
		do { 


		    ListItem* first = curStackItem.getFirst();
		    wchar_t label = first->getNextChar();

		    
		    stack_.getWord().resize( getCurDepth() + 1 );
		    stack_.getWord().at( getCurDepth() ) = label;
		    // std::wcout<<"word: "<<stack_.getWord()<<std::endl;
		    
		    nextStackItem.setFilterPos( dic_.walk( curStackItem.getFilterPos(), label ) ); // this may well be 0
		    
		    while( first && first->getNextChar() == label ) {
			
			if( Position* pos = dynamic_cast< Position* >( first ) ) {
			    StateId_t nextState = dic_.walk( pos->getState(), label );
			    Position* newPos = new Position( nextState, dic_.getSusoString( nextState ), pos->hasError() );
			    if( newPos->getNextChar() != 0 ) {
				nextStackItem.addItem( newPos );
			    }
			    if( StateId_t newState = dic_.walkStr( nextState, patternFrom_.c_str() ) ) {
				nextStackItem.addNewState( newState );
			    }
			    
			    if( newPos->hasError() &&
				dic_.isFinal( nextState ) && 
				! filterDic_.isFinal( nextStackItem.getFilterPos() ) ) {
				foundFinal = true;
			    }
			    
			    pos->stepToNextChar();
			}

			else if( PatternTracer* patTracer = dynamic_cast< PatternTracer* >( first ) ) {
			    
			    if( *( patTracer->getPatternPos() + 1 ) == 0 ) { // if pattern read completely
				std::vector< StateId_t >::const_iterator it = stack_.at( getCurDepth() - patternTo_.size() +1 ).getNewStates().begin();
				std::vector< StateId_t >::const_iterator end = stack_.at( getCurDepth() - patternTo_.size() + 1 ).getNewStates().end();
				for( ; it != end; ++it ) {
				    Position* newPos = new Position( *it, dic_.getSusoString( *it ), true );
				    if( newPos->getNextChar() != 0 ) {
					nextStackItem.addItem( newPos );
				    }
				    if( dic_.isFinal( *it ) )
					foundFinal = true;
				}
			    }
			    else {
				PatternTracer* newTracer = new PatternTracer( patTracer->getPatternPos() + 1 );
				if( newTracer->getNextChar() != 0 ) {
				    nextStackItem.addItem( newTracer );
				}
			    }
			    patTracer->stepToNextChar();
			    
			}
			else {
			    throw exceptions::cslException( "VERY STRANGE" );
			}
			
			curStackItem.arrangeFirst(); // re-arrange list of ListItems
			first = curStackItem.getFirst(); // get the new 1st item
		    } // for all items with same label

		    if( nextStackItem.getNewStates().size() > 0 ) {
			nextStackItem.addItem( new PatternTracer( patternTo_.c_str() ) );
		    }
		} while( !foundFinal & nextStackItem.empty() && ! curStackItem.empty() );

		if( nextStackItem.empty() ) stack_.back();
		else stack_.forward();

	    } while( !foundFinal && isGood() );
	    
	    if( isGood() ) ++tokenCount_;

	    return isGood();
	}
	
    private:
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

    
    
