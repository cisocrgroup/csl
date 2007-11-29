#include<iostream>

#include<vector>
#include<stack>
#include<set>

#include "../Global.h"
#include "../MinDic/MinDic.h"

namespace csl {

    class PatternApplier {
    private:
	typedef MinDic<>::State MDState_t;

	/**
	 * We need this only for a depth-first traversal
	 * This is a very unimportant class!
	 */
	class TraversalItem {
	public:
	    TraversalItem( const MDState_t& patState__, const MDState_t& dicState__ ) :
		patState( patState__ ), dicState( dicState__ ) {
	    }
	    MDState_t patState;
	    MDState_t dicState;
	};
	
	/**
	 * @brief Error is usually stored with a Position and represents one error operation 
	 * that was used during the long life of the Position
	 */
	class Error {
	public:
	    Error() :
		patternID_( -1 ),
		position_( -1 ) {
	    }


	    Error( size_t errorPattern, size_t position ) :
		patternID_( errorPattern ),
		position_( position ) {
	    }

	    void set( size_t errorPattern, size_t position ) {
		patternID_ = errorPattern;
		position_ = position;
	    }
	    bool isSet() const {
		return position_ != -1;
	    }
	    
	    size_t getPosition() const {
		return position_;
	    }
	    size_t getPatternID() const {
		return patternID_;
	    }
	private:
	    /**
	     * An index pointing to the array of patterns
	     */
	    int patternID_;
	    /**
	     * The char offset in the word where the error was inserted
	     */
	    int position_;
	};

	class ListItem {
	public:
	    inline ListItem() : 
		nextItem_( 0 ) {
	    }

	    inline virtual ~ListItem() {
	    }
	    
	    bool operator<( const ListItem& other ) const {
		if( getNextChar() == 0 ) {
		    return false; // sort those objects to the very end
		}
		else if( other.getNextChar() == 0 ) {
		    return true; // sort those objects to the very end
		}

		return this->getNextChar() < other.getNextChar();
	    }
	    virtual bool isPosition() const = 0;
	    virtual bool isPatternTracer() const = 0;

	    virtual wchar_t getNextChar() const = 0;
	    virtual bool stepToNextChar() = 0;

	    virtual const MDState_t& getState() const = 0;

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
		
	    inline Position( const MDState_t& state, const Error& error );

	    inline virtual ~Position() {}

	    inline bool isPosition() const {
		return true;
	    }

	    inline bool isPatternTracer() const {
		return false;
	    }

	    virtual const MDState_t& getState() const;

	    inline size_t getPHValue() const;

//	    inline void set(  StateId_t state, const wchar_t* nextChar, const Error& error, size_t perfHashValue = 0 );

	    inline size_t hasError() const;
	    
	    inline const Error& getError() const {
		return error_;
	    }

	    inline wchar_t getNextChar() const;

	    inline bool stepToNextChar();

	    

	private:
	    MDState_t state_;
	    const wchar_t* nextChar_;
	    Error error_;
	}; // class Position


	/**
	 * Objects of this class are used to keep track of when the error pattern can be applied.
	 */
	class PatternTracer : public ListItem {
	public:
	    /**
	     * Gives the tracer a standard initialisation
	     */
	    inline PatternTracer() :
		state_( 0 ),
		nextChar_( 0 ),
		depth_( 0 ) {
	    }

	    /**
	     * Initialises a new tracer with given values
	     */
	    inline PatternTracer( const MDState_t& state, size_t depth ) :
		state_( state ),
		depth_( depth ) {
		nextChar_ = state.getSusoString();

		// is that for deletions???
		if( *nextChar_ == PatternApplier::patternDelimiter_ ) {
		    ++nextChar_;
		}
	    }

	    /**
	     * A do-nothing destructor
	     */
	    inline ~PatternTracer() {}


	    /**
	     * sets all values
	     */
// 	    void set( StateId_t state, const wchar_t* nextChar, size_t depth, size_t perfHashValue = 0 ) {
// 		state_ = state;
// 		nextChar_ = nextChar;
// 		depth_ = depth;
// 		perfHashValue_ = perfHashValue;
// 	    }
    
	    /**
	     * returns if this ListItem is a Position, which is: false
	     */
	    inline bool isPosition() const {
		return false;
	    }

	    /**
	     * returns if this ListItem is a PatternTracer, which is: true
	     */
	    inline bool isPatternTracer() const {
		return true;
	    }

	    /**
	     * returns the current character 
	     */
	    inline wchar_t getNextChar() const {
		return *nextChar_;
	    }

	    /**
	     *
	     */
	    bool stepToNextChar() {
		if( *nextChar_ == 0 ) return false;
		++nextChar_;
		if( *nextChar_ == PatternApplier::patternDelimiter_ ) {
		    std::wcout<<"HA"<<std::endl;
		    ++nextChar_;
		    if( *nextChar_ == 0 ) return false;
		}
		return true;
	    }

	    /**
	     * 
	     */
	    virtual const MDState_t& getState() const {
		return state_;
	    }
	    
	    inline size_t getDepth() const {
		return depth_;
	    }
	    
// 	    size_t getPHValue() const {
// 		return perfHashValue_;
// 	    }
	private:
	    /**
	     * the current state inside the mdic of all patterns
	     */
	    MDState_t state_;

	    /**
	     * the current position in the state_'s susoString
	     */
	    const wchar_t* nextChar_;

	    /**
	     * ?
	     */
	    size_t depth_;

	}; // class PatternTracer


	/**
	 * A StackItem represents one item of the stack that controls the traversal-backtracking-procedure of the dictionary
	 */
	class StackItem {
	public:
	    StackItem() :
		list_( 0 )
		{
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
		list_ = 0;
		
	    }

	    void addItem( ListItem* newItem ) {
		ListItem* listPos = list_;
		ListItem* lastPos = 0;
		while( ( listPos != 0 ) && ( *listPos < *newItem ) ) {
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
	    
	    const ListItem* getFirst() const {
		return list_;
	    }
	    
	    void arrangeFirst() {
		// log out first item and add it again
		ListItem* first = list_;
		list_ = list_->getNextItem();

		addItem( first );
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

	    bool isGood() const {
		return ( getFirst() && ( getFirst()->getNextChar() != 0 ) );
	    }


	private:
	    ListItem* list_;
	    StateId_t filterPos_;
	};


	/**
	 * the stack that controls the traversal-backtracking-procedure of the dictionary
	 */
	class Stack : public std::vector< StackItem > {
	public:
	    Stack() :
		std::vector< StackItem >( 2 ),
		depth_( 0 ) {
		reserve( Global::lengthOfStr ); // do not allow realloc - very ugly !!!
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
	PatternApplier( const MinDic< int >& dic, const MinDic< int >& filterDic, const char* patternFile ) :
	    dic_( dic ),
	    filterDic_( filterDic ),
	    isGood_( false ){
	    
	    loadPatterns( patternFile );

	    // insert initial positions and patTracers
	    stack_.at( 0 ).addItem( new Position( MDState_t( dic_ ), Error() ) );
	    stack_.at( 0 ).addItem( new PatternTracer( MDState_t( patternGraph_ ), 0 ) );

	    tokenCount_ = 0;
	    next();

	    // std::wcerr<<"New PatternApplier with: "<<patternFrom_<<"->"<<patternTo_<<std::endl;
	}

	void loadPatterns( const char* patternFile );

	void printMe() {
	    std::wcout<<"print me "<<std::endl; 
	}

	const std::wstring& getWord() const {
	    return ( stack_.getWord() );
	}

	bool isGood() const {
	    return isGood_;
	}

	bool next();
	
	const wchar_t* getPattern() const {
	    return patterns_.at( curError_.getPatternID() ).c_str();
	}

	const size_t getErrorPos() const {
	    return curError_.getPosition();
	}

	static const wchar_t patternDelimiter_ = L'_';

    private:
	size_t getCurDepth() const {
	    return stack_.getDepth();
	}


	Stack stack_;
	const MinDic< int >& dic_;
	const MinDic< int >& filterDic_;
	
	MinDic< int > patternGraph_;
	std::vector< std::wstring > patterns_;

	Error curError_;
	size_t tokenCount_;
	bool isGood_;

    }; // class PatternApplier


    bool PatternApplier::next() {
	isGood_ = true;
	bool foundFinal = false;

	do { // this loop terminates at a final state
	    // std::cerr<<"Enter final loop"<<std::endl;


	    while( isGood() && ! stack_.at( getCurDepth() ).isGood() ) {
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
		    
		bool addedTriggerPosition = false;
		while( first && first->getNextChar() == label  ) {
		    Position* pos;
		    PatternTracer* patTracer;

		    if( ( pos = dynamic_cast< Position* >( first ) ) ) {
			MDState_t nextState( (*pos).getState().getTransTarget( label ) );
			Position* newPos = new Position( nextState, pos->getError() );
			nextStackItem.addItem( newPos );
			// The 2nd constraint has to be dropped if insertions are allowed
			if( ! newPos->hasError() && ( newPos->getNextChar() != 0 ) ) addedTriggerPosition = true;
			    
			if( newPos->hasError() &&
			    nextState.isFinal() && 
			    ! filterDic_.lookup( stack_.getWord().c_str() ) ) {

			    curError_ = newPos->getError();
			    foundFinal = true;
			}
			    
			pos->stepToNextChar();
		    }

		    else if( ( patTracer = dynamic_cast< PatternTracer* >( first ) ) ) {
			MDState_t nextState = (*patTracer).getState().getTransTarget( label );

			PatternTracer* newTracer = new PatternTracer( nextState, patTracer->getDepth() + 1 );

			// if pattern-right-side read completely
			if( nextState.hasTransition( patternDelimiter_ ) ) {
			    MDState_t patStartState = nextState.getTransTarget( patternDelimiter_ );

			    std::stack< TraversalItem > stack;

			    ListItem* item = stack_.at( getCurDepth() - patTracer->getDepth() ).getFirst();
			    while( item ) {
				if( item->isPosition() &&
				    ! ((Position*)item)->hasError()
				    ) {
				    stack.push( TraversalItem( patStartState, item->getState() ) );
				}
				item = item->getNextItem();
			    }

			    while( ! stack.empty() ) {
				const MDState_t& patState = stack.top().patState;
				const MDState_t& dicState = stack.top().dicState;

				MDState_t newDicState( dic_ );
				stack.pop();
				for( const wchar_t* c = patState.getSusoString();
				     *c != 0; ++c ) {
				    if( dicState.hasTransition( *c ) ) {
					MDState_t newDicState = dicState.getTransTarget( *c );
					MDState_t newPatState = patState.getTransTarget( *c );
					if( newPatState.isFinal() ) {
					    Position* newPos = new Position( newDicState, Error( newPatState.getPerfHashValue(), getCurDepth() - patTracer->getDepth() ) );
					    nextStackItem.addItem( newPos );
					    // handle addedTriggerPosition here if allowing more than one error
					    if( newDicState.isFinal() ) {
						foundFinal = true;
						curError_ = newPos->getError();
					    }
					}
					
					// push new pair only if newPatState has some outgoing transitions
					if( *( newPatState.getSusoString() ) ) {
					    stack.push( TraversalItem( newPatState, newDicState ) );
					}
				    } // if could walk in dictionary
				} // for all transitions from patState
			    } // while stack not empty
				
			} // if pattern-right-side read completely
				
			nextStackItem.addItem( newTracer );
			patTracer->stepToNextChar();
				
		    }
			
		    curStackItem.arrangeFirst(); // re-arrange list of ListItems
		    first = curStackItem.getFirst(); // get the new 1st item
		} // for all items with same label

		if( addedTriggerPosition ) {
		    nextStackItem.addItem( new PatternTracer( MDState_t( patternGraph_ ), 0 ) );
		}
	    } while( !foundFinal && ! nextStackItem.isGood() && curStackItem.isGood() );

	    if( nextStackItem.isGood() ) stack_.forward();
	    else if( ! curStackItem.isGood() ) stack_.back();
		
	} while( !foundFinal && isGood() );
	    
	if( isGood() ) ++tokenCount_;

	return isGood();
    } // next()

    void PatternApplier::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	
	std::wstring line;
	while( getline( fi, line ) ) {
	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( "ErrDicConstructor: Invalid line in pattern file" );
	    }
	    patterns_.push_back( line.substr( delimPos + 1 ) + patternDelimiter_ + line.substr( 0, delimPos ) );
	    
	    
	}
	fi.close();
	std::sort( patterns_.begin(), patterns_.end() );
	patternGraph_.initConstruction();
	for( std::vector< std::wstring >::const_iterator it = patterns_.begin(); it != patterns_.end(); ++it ) {
	    patternGraph_.addToken( it->c_str(), 0 );
	}
	patternGraph_.finishConstruction();

	patternGraph_.writeToFile( "./patterns.mdic" );
    }


} // eon

#include "./Position.tcc"

    
    
    
