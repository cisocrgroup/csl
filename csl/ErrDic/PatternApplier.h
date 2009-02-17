#include<iostream>

#include<vector>
#include<stack>
#include<set>
#include<algorithm>
#include<errno.h>

#include "../Global.h"
#include "../Stopwatch.h"
#include "../MinDic/MinDic.h"
#include "../LevDEA/LevDEA.h"

namespace csl {

    /**
     * @brief The central class for the construction of error/variant dicionaries in alphabetical order
     */
    class PatternApplier {
    private:
	class Position;

	typedef MinDic<>::State MDState_t;

	/**
	 * We need this only for a depth-first traversal
	 * This is a very unimportant class!
	 */
	class TraversalItem {
	public:

	    TraversalItem( const Position& position__, const MDState_t& patState__, const MDState_t& dicState__ ) :
		position( position__ ),
		patState( patState__ ),
		dicState( dicState__ ) {
	    }
	    const Position& position;
	    MDState_t patState;
	    MDState_t dicState;
	};
	
	/**
	 * @brief Error is usually stored with a Position and represents one error operation 
	 * that was used during the long life of the Position
	 */
	class Error {
	public:
	    Error();

	    Error( size_t errorPattern, size_t position );

	    void set( size_t errorPattern, size_t position );

	    size_t getPosition() const;

	    size_t getPatternID() const;
	    
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
	    
	    // Do we want this ???
// 	    inline ListItem( const ListItem& other ) :
// 		nextItem_( 0 ) {
// 	    }

	    bool operator<( const ListItem& other ) const {
		if( getNextChar() == 0 ) {
		    return false; // sort *this to the very end
		}
		else if( other.getNextChar() == 0 ) {
		    return true; // sort 'other' to the very end
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

	    virtual void print( std::wostream& os = std::wcout ) const = 0;

	private:
	    ListItem* nextItem_;

	}; // class ListItem


	/**
	 * This class represents a position in the dictionary automaton. One or more of those objects 
	 * form one stack item for the unified traversal of different branches of the automaton.
	 */
	class Position : public ListItem {
	public:
	    inline Position();
	    
 	    inline explicit Position( const MDState_t& state );

	    inline virtual ~Position() {}

	    /**
	     * returns if this ListItem is a Position, which is: true
	     */
	    inline bool isPosition() const {
		return true;
	    }


	    /**
	     * returns if this ListItem is a PatternTracer, which is: false
	     */
	    inline bool isPatternTracer() const {
		return false;
	    }

	    virtual const MDState_t& getState() const;

	    inline size_t getPHValue() const;

//	    inline void set(  StateId_t state, const wchar_t* nextChar, const Error& error, size_t perfHashValue = 0 );

	    inline size_t hasErrors() const;

	    inline size_t getNrOfErrors() const {
		return errors_.size();
	    }
	    
	    inline const std::vector< Error >& getErrors() const {
		return errors_;
	    }

	    /**
	     * Add all errors in the range [it, end) to the list of errors
	     */
	    template< typename I >
	    void addErrors( I it, I end ) {
		for( ; it != end; ++it ) {
		    addError( *it );
		}
	    }

	    void addError( const Error& e ) {
		errors_.push_back( e );
	    }

	    inline wchar_t getNextChar() const;

	    inline bool stepToNextChar();

	    inline void print( std::wostream& os = std::wcout ) const {
		os << L"(POS:st=" << state_.getStateID() << L",c=" << *nextChar_ << ",";
		for( std::vector< Error >::const_iterator it = errors_.begin(); it != errors_.end(); ++it ) {
		    os << L"<" << it->getPatternID() << ">";
		}
		os << L")";
	    }

	private:
	    MDState_t state_;
	    const wchar_t* nextChar_;
	    std::vector< Error > errors_;

	}; // class Position


	/**
	 * Objects of this class are used to keep track of when the error pattern can be applied.
	 */
	class PatternTracer : public ListItem {
	public:
	    /**
	     * Gives the tracer a standard initialisation
	     */
	    inline PatternTracer();

	    /**
	     * Initialises a new tracer with given values
	     */
	    inline PatternTracer( const MDState_t& state, size_t depth );

	    /**
	     * A do-nothing destructor
	     */
	    inline ~PatternTracer();

	    /**
	     * returns if this ListItem is a Position, which is: false
	     */
	    inline bool isPosition() const;

	    /**
	     * returns if this ListItem is a PatternTracer, which is: true
	     */
	    inline bool isPatternTracer() const;

	    /**
	     * returns the current character 
	     */
	    inline wchar_t getNextChar() const;

	    /**
	     *
	     */
	    bool stepToNextChar();

	    /**
	     * 
	     */
	    virtual const MDState_t& getState() const;
	    
	    inline size_t getDepth() const;
	    
	    inline void print( std::wostream& os = std::wcout ) const {
		os << "(TRA:" << "st=" << state_.getStateID() << ",c=" << *nextChar_ << ")";
	    }

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
		list_( 0 ),
		constraintPos_( 0 )
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

	    void setConstraintPos( StateId_t pos ) {
		constraintPos_ = pos;
	    }

	    StateId_t getConstraintPos() const {
		return constraintPos_;
	    }

	    bool empty() const {
		return list_ == 0;
	    }

	    bool isGood() const {
		return ( getFirst() && ( getFirst()->getNextChar() != 0 ) );
	    }

	    inline void print( std::wostream& os = std::wcout ) const {
		ListItem* l = list_;
		while( l ) {
		    l->print();
		    l = l->getNextItem();
		}
	    }


	private:
	    ListItem* list_;
	    StateId_t constraintPos_;

	}; // class StackItem


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

	    inline void print( std::wostream& os = std::wcout ) const {
		size_t i = 0;
		for( const_iterator items = begin(); items != end(); ++items ) {
		    os << i << ".\t";
		    items->print();
		    os << std::endl;
		    ++i;
		}
	    }
	    

    

	private:
	    size_t depth_;
	    std::wstring word_;
	}; // class Stack


    public:
	typedef std::vector< Error >::const_iterator ErrorIterator_t;

	PatternApplier( const MinDic< int >& dic, const char* patternFile ) :
	    dic_( dic ),
	    filterDic_( 0 ),
	    constraintDic_( 0 ),
	    isInitialised_( false ),
	    isGood_( false ),
	    minNrOfErrors_( 1 ),
	    maxNrOfErrors_( 5000 ) {
	    
	    loadPatterns( patternFile );


	    // std::wcerr<<"New PatternApplier with: "<<patternFrom_<<"->"<<patternTo_<<std::endl;
	}

	void init() {
	    // insert initial positions and patTracers
	    stack_.at( 0 ).addItem( new Position( MDState_t( dic_ ) ) );
	    stack_.at( 0 ).addItem( new PatternTracer( MDState_t( patternGraph_ ), 0 ) );

	    if( constraintDic_ ) {
		stack_.at( 0 ).setConstraintPos( constraintDic_->getRoot() );
	    }
	    tokenCount_ = 0;
	    isInitialised_ = true;
	    next();
	}

	void constructErrDic(  ErrDic& errDic );

	void setFilterDic( const MinDic< int >& filterDic ) {
	    filterDic_ = &filterDic;
	}

	void setConstraintDic( const MinDic< int >& constraintDic ) {
	    constraintDic_ = &constraintDic;
	}

	void setMinNrOfErrors( size_t minNrOfErrors ) {
	    minNrOfErrors_ = minNrOfErrors;
	}

	void setMaxNrOfErrors( size_t maxNrOfErrors ) {
	    maxNrOfErrors_ = maxNrOfErrors;
	}

	void loadPatterns( const char* patternFile );

	const std::wstring& getWord() const {
	    return ( stack_.getWord() );
	}

	std::wstring getBaseWord() const {
	    std::wstring baseWord( getWord() );
	    throw exceptions::cslException( "PatternApplier::getBaseWord not yet implemented" );
	    return baseWord;
	}

	bool isGood() const {
	    return isGood_;
	}

	bool next();
	
	const std::vector< std::vector< Error > >& getErrors() const {
	    return curErrors_;
	}


	inline void printPatterns( std::wostream& os = std::wcout ) const {
	    for( std::vector< std::vector< Error > >::const_iterator reading = curErrors_.begin();
		 reading != curErrors_.end();
		 ++reading ) {
		os<< L"[";
		for( std::vector< Error >::const_iterator error = reading->begin();
		     error != reading->end();
		     ++error ) {
		    os<<L"(" << patterns_.at( error->getPatternID() ) << L", " << error->getPosition() << L")";
		}
	    os << L"]";
	}
	}

	inline void printCurrent( std::wostream& os = std::wcout ) const {
	    os<<getWord()<<"|";
	    printPatterns( os );
	    os<<std::endl;
	}

	static const wchar_t patternDelimiter_ = L'_';

    private:
	size_t getCurDepth() const {
	    return stack_.getDepth();
	}

	void addNewBranches( const MDState_t& patStartState, size_t deltaDepth, bool* addedTriggerPosition, bool* foundFinal );

	Stack stack_;
	const MinDic< int >& dic_;
	const MinDic< int >* filterDic_;
	const MinDic< int >* constraintDic_;
	
	bool isInitialised_;

	MinDic< int > patternGraph_;
	std::vector< std::wstring > patterns_;

	std::vector< std::vector< Error > > curErrors_;
	size_t tokenCount_;
	bool isGood_;

	size_t minNrOfErrors_;
	size_t maxNrOfErrors_;

    }; // class PatternApplier



    bool PatternApplier::next() {

	if( ! isInitialised_ ) {
	    throw exceptions::LogicalError( "csl::PatternApplier: Called next() before calling init() " );
	}

	isGood_ = true;
	bool foundFinal = false;
	curErrors_.clear();

	do { // this loop terminates at a final state
	    // std::cerr<<"Enter final loop"<<std::endl;


	    while( isGood() && ! stack_.at( getCurDepth() ).isGood() ) {
		isGood_ = stack_.back();
	    }

	    if( ! isGood_ ) return false;

	    StackItem& curStackItem = stack_.at( getCurDepth() );
	    StackItem& nextStackItem = stack_.at( getCurDepth() + 1 );
		    
		
	    do { // while no continuation could be found and the curStackItem is still good

		ListItem* first = curStackItem.getFirst();
		wchar_t label = first->getNextChar();
		    
		bool constraintFailed = false;

		if( constraintDic_ ) {
		    StateId_t newConstraintPos = constraintDic_->walk( curStackItem.getConstraintPos(), label );
		    if( newConstraintPos ) {
			nextStackItem.setConstraintPos( newConstraintPos );
			constraintFailed = false;
		    }
		    else {
			constraintFailed = true;
		    }
		}


		stack_.getWord().resize( getCurDepth() + 1 );
		stack_.getWord().at( getCurDepth() ) = label;

//		stack_.print(); std::wcout<<"word: "<<stack_.getWord()<<std::endl; std::wcout<<"------"<<std::endl; // DEBUG

		/*
		 * The variable addedTriggerPosition indicates if a new Position was 
		 * inserted which shall trigger the insertion of a new PatternTracer.
		 * This is not the case e.g. if the new Position has already the 
		 * max. number of errors.
		 */
		bool addedTriggerPosition = false;
		
		// for all items with the same label
		while( first && ( first->getNextChar() == label ) ) {
		    Position* pos;
		    PatternTracer* patTracer;
		    
		    if( constraintFailed ) {
			// do nothing at all
		    }
		    else if( ( pos = dynamic_cast< Position* >( first ) ) ) {
			MDState_t nextState( (*pos).getState().getTransTarget( label ) );
			Position* newPos = new Position( nextState );
			// copy all errors of pos to newPos
			newPos->addErrors( pos->getErrors().begin(), pos->getErrors().end() );
			
			nextStackItem.addItem( newPos );
			
			if( ( newPos->getNrOfErrors() < maxNrOfErrors_ ) &&
			    ( newPos->getNextChar() != 0 ) // has to be dropped if insertions are allowed
			    ) {
			    addedTriggerPosition = true;
			}
			
			if( newPos->getErrors().size() >= minNrOfErrors_ &&
			    nextState.isFinal() && 
			    ! ( filterDic_ && filterDic_->lookup( stack_.getWord().c_str() ) ) &&
			    ( ! constraintDic_ || constraintDic_->isFinal( nextStackItem.getConstraintPos() ) ) ) {
			    
			    curErrors_.push_back( newPos->getErrors() );
			    foundFinal = true;
			}
		    } // if first item is a Position
		    else if( ( patTracer = dynamic_cast< PatternTracer* >( first ) ) ) {
			MDState_t nextState = (*patTracer).getState().getTransTarget( label );
			
			PatternTracer* newTracer = new PatternTracer( nextState, patTracer->getDepth() + 1 );
			
			// if pattern-right-side read completely
			if( nextState.hasTransition( patternDelimiter_ ) ) {
			    MDState_t patStartState = nextState.getTransTarget( patternDelimiter_ );
 			    addNewBranches( patStartState, patTracer->getDepth(), &addedTriggerPosition, &foundFinal );
			} // if pattern-right-side read completely
			
			nextStackItem.addItem( newTracer );
			
		    } // if first item is a pattern tracer
		    
		    first->stepToNextChar();
			
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

    void PatternApplier::addNewBranches( const MDState_t& patStartState, size_t deltaDepth, bool* addedTriggerPosition, bool* foundFinal ) {
	std::stack< TraversalItem > stack;

	ListItem* item = stack_.at( getCurDepth() - deltaDepth ).getFirst();
	while( item ) {
	    if( item->isPosition() &&
		( ((Position*)item)->getNrOfErrors() < maxNrOfErrors_ )
		) {
		stack.push( TraversalItem( (const Position&)(*item), patStartState, item->getState() ) );
	    }
	    item = item->getNextItem();
	}
	
	while( ! stack.empty() ) {
	    const MDState_t patState = stack.top().patState; // by value, because the TraversalItem will be destroyed with the next pop()
	    const MDState_t dicState = stack.top().dicState; // by value, see above
	    const Position& position = stack.top().position; // by reference: also in the TraversalItem there's just a reference to the Pos object 
	    
   
	    MDState_t newDicState( dic_ );
	    stack.pop();
	    for( const wchar_t* c = patState.getSusoString();
		 *c != 0; ++c ) {
		if( dicState.hasTransition( *c ) ) {
		    MDState_t newDicState = dicState.getTransTarget( *c );
		    MDState_t newPatState = patState.getTransTarget( *c );
		    if( newPatState.isFinal() ) {
			Position* newPos = new Position( newDicState );

			newPos->addErrors( position.getErrors().begin(), position.getErrors().end() );
			newPos->addError( Error( newPatState.getPerfHashValue(), getCurDepth() - deltaDepth ) );

			stack_.at( getCurDepth() + 1 ).addItem( newPos );
					    
			if( ( newPos->getNrOfErrors() < maxNrOfErrors_ ) &&
			    ( newPos->getNextChar() != 0 ) // has to be dropped if insertions are allowed
			    ) {
			    *addedTriggerPosition = true;
			}

			if( newPos->getErrors().size() >= minNrOfErrors_ &&
			    newDicState.isFinal() && 
			    ! ( filterDic_ && filterDic_->lookup( stack_.getWord().c_str() ) ) &&
			    ( ! constraintDic_ || constraintDic_->isFinal( stack_.at( getCurDepth() + 1 ).getConstraintPos() ) ) ) {
 			    
			    *foundFinal = true;
			    curErrors_.push_back( newPos->getErrors() );
			}
		    }
		    
		    // push new pair only if newPatState has some outgoing transitions
		    if( *( newPatState.getSusoString() ) ) {
			stack.push( TraversalItem( position, newPatState, newDicState ) );
		    }
		} // if could walk in dictionary
	    } // for all transitions from patState
	} // while stack not empty
				
    }

    void PatternApplier::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	if( ! fi ) {
	    throw exceptions::badFileHandle( "PatternApplier: Couldn't open pattern file" );
	}
	
	std::wstring line;
	while( getline( fi, line ) ) {
	    if( errno == EILSEQ ) { // if failbit is set BEFORE eof
		throw exceptions::badInput( "PatternApplier::loadPatterns: Encoding error in input sequence." );
	    }

	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( "PatternApplier: Invalid line in pattern file" );
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

    void PatternApplier::constructErrDic(  ErrDic& errDic ) {
	
	Stopwatch watch;
	watch.start();

	init();
	try {

	    errDic.initConstruction();
	    size_t nrOfTokens = 0;

	    while( isGood() ) {
		printCurrent( std::wcout );

		if( ! ( ++nrOfTokens % 500000 ) ) {
		    std::wcerr<<nrOfTokens / 1000<<"k. "<<watch.readMilliseconds()<<" ms"<< std::endl;
		    watch.start();
		    printCurrent( std::wcerr );
		}

//		errDic.addToken( getWord().c_str(), L"elefant", L"x y" );
		
		next();
	    }
	    
	    errDic.finishConstruction();
	    
	} catch( exceptions::badInput exc ) {
	    std::wcout<<"ErrDic Creation failed: "<<exc.what()<<std::endl; 
	}
    }

} // eon

#include "./Position.tcc"
#include "./PatternTracer.tcc"
#include "./Error.tcc"

    
    
    
