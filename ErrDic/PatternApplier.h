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
	 * We need this only for a depth-first traversal
	 * This is a very unimportant class!
	 */
	class TraversalItem {
	public:
	    TraversalItem( StateId_t p, StateId_t d, size_t dphv, size_t ptphv ) :
		patState( p ), dicState( d ), dicPHValue( dphv ), patTracerPHValue( ptphv ) {
	    } 
	    StateId_t patState;
	    StateId_t dicState;
	    size_t dicPHValue;
	    size_t patTracerPHValue;
	};
	
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
	    int patternID_;
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

	    virtual StateId_t getState() const = 0;

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
		
	    inline Position( StateId_t state, const wchar_t* nextChar, const Error& error, size_t perfHashValue = 0 );

	    inline virtual ~Position() {}

	    inline bool isPosition() const {
		return true;
	    }

	    inline bool isPatternTracer() const {
		return false;
	    }

	    virtual StateId_t getState() const;

	    inline size_t getPHValue() const;

	    inline void set(  StateId_t state, const wchar_t* nextChar, const Error& error, size_t perfHashValue = 0 );

	    inline size_t hasError() const;

	    inline const Error& getError() const {
		return error_;
	    }

	    inline wchar_t getNextChar() const;

	    inline bool stepToNextChar();

	    

	private:
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    Error error_;
	    size_t perfHashValue_;
	}; // class Position


	/**
	 * Objects of this class are used to keep track of when the error pattern can be applied.
	 */
	class PatternTracer : public ListItem {
	public:
	    inline PatternTracer() :
		state_( 0 ),
		nextChar_( 0 ),
		depth_( 0 ) {
	    }

	    inline PatternTracer( StateId_t state, const wchar_t* nextChar, size_t depth, size_t perfHashValue = 0 ) :
		state_( state ),
		nextChar_( nextChar),
		depth_( depth ),
		perfHashValue_( perfHashValue ) {
		if( *nextChar_ == PatternApplier::patternDelimiter_ ) {
		    ++nextChar_;
		}
	    }

	    inline ~PatternTracer() {}

	    void set(  StateId_t state, const wchar_t* nextChar, size_t depth, size_t perfHashValue = 0 ) {
		state_ = state;
		nextChar_ = nextChar;
		depth_ = depth;
		perfHashValue_ = perfHashValue;
	    }
    
	    inline bool isPosition() const {
		return false;
	    }

	    inline bool isPatternTracer() const {
		return true;
	    }

	    inline wchar_t getNextChar() const {
		return *nextChar_;
	    }

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

	    virtual StateId_t getState() const {
		return state_;
	    }

	    inline size_t getDepth() const {
		return depth_;
	    }

	    size_t getPHValue() const {
		return perfHashValue_;
	    }
	private:
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    size_t depth_;
	    size_t perfHashValue_;
	}; // class PatternTracer



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
	PatternApplier( const MinDic< int >& dic, const MinDic< int >& filterDic, const char* patternFile ) :
	    dic_( dic ),
	    filterDic_( filterDic ),
	    isGood_( false ){
	    
	    loadPatterns( patternFile );

	    
	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    // insert initial positions and patTracers
	    stack_.at( 0 ).addItem( new Position( st, suso, Error(), 0 ) );
	    stack_.at( 0 ).addItem( new PatternTracer( patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0, 0 ) );

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

	bool next() {
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
			    size_t dicPHValue = pos->getPHValue();
 			    StateId_t nextState = dic_.walkPerfHash( pos->getState(), label, dicPHValue );
			    // note that dic_.walkPerfHash() updates dicPHValue
 			    Position* newPos = new Position( nextState, dic_.getSusoString( nextState ), pos->getError(), dicPHValue );
			    nextStackItem.addItem( newPos );
			    // The 2nd constraint has to be dropped if insertions are allowed
			    if( ! newPos->hasError() && ( newPos->getNextChar() != 0 ) ) addedTriggerPosition = true;
			    
			    if( newPos->hasError() &&
				dic_.isFinal( nextState ) && 
				! filterDic_.lookup( stack_.getWord().c_str() ) ) {
				dicPHValue_ = dicPHValue;
				curError_ = &( newPos->getError() );
				foundFinal = true;
			    }
			    
			    pos->stepToNextChar();
			}
 			else if( ( patTracer = dynamic_cast< PatternTracer* >( first ) ) ) {
			    size_t perfHashValue = patTracer->getPHValue();
 			    StateId_t nextState = patternGraph_.walkPerfHash( patTracer->getState(), label, perfHashValue );
 			    PatternTracer* newTracer = new PatternTracer( nextState, patternGraph_.getSusoString( nextState ), patTracer->getDepth() + 1, perfHashValue );
			    // if pattern-right-side read completely
			    StateId_t patStartState;
 			    if( ( patStartState = patternGraph_.walkPerfHash( nextState, patternDelimiter_, perfHashValue ) ) ) {
				

				std::stack< TraversalItem > stack;

				
				
				ListItem* item = stack_.at( getCurDepth() - patTracer->getDepth() ).getFirst();
				while( item ) {
				    if( item->isPosition() &&
					! ((Position*)item)->hasError()
					) {
					stack.push( TraversalItem( patStartState, item->getState(), ((Position*)item)->getPHValue(), perfHashValue ) );
				    }
				    item = item->getNextItem();
				}

				while( ! stack.empty() ) {
				    StateId_t patState = stack.top().patState;
				    StateId_t dicState = stack.top().dicState;

				    size_t dicPHValue = stack.top().dicPHValue;
				    size_t newDicPHValue = dicPHValue;

				    size_t patTracerPHValue = stack.top().patTracerPHValue;
				    size_t newPatTracerPHValue = patTracerPHValue;

				    StateId_t newDicState = 0;
				    stack.pop();
				    for( const wchar_t* c = patternGraph_.getSusoString( patState );
					 *c != 0; ++c ) {
					// reset those as they're changed by the walk-method
					newDicPHValue = dicPHValue; 
					newPatTracerPHValue = patTracerPHValue;

					if( ( newDicState = dic_.walkPerfHash( dicState, *c, newDicPHValue ) ) ) {
					    StateId_t newPatState = patternGraph_.walkPerfHash( patState, *c, newPatTracerPHValue );
					    if( patternGraph_.isFinal( newPatState ) ) {
						Position* newPos = new Position( newDicState, dic_.getSusoString( newDicState ), Error( newPatTracerPHValue, getCurDepth() - patTracer->getDepth() ), newDicPHValue );
						nextStackItem.addItem( newPos );
						// handle addedTriggerPosition here if allowing more than one error
						if( dic_.isFinal( newDicState ) ) {
						    foundFinal = true;
						    dicPHValue_ = newDicPHValue;
						    curError_ = &( newPos->getError() );
						}
					    }

					    // push new pair only if newPatState has some outgoing transitions
					    if( *( patternGraph_.getSusoString( newPatState ) ) ) {
						stack.push( TraversalItem( newPatState, newDicState, newDicPHValue, newPatTracerPHValue ) );
					    }
					}
				    }
				}
				
 			    } // if pattern-right-side read completely
				
			    nextStackItem.addItem( newTracer );
			    patTracer->stepToNextChar();
				
			}
			
			curStackItem.arrangeFirst(); // re-arrange list of ListItems
			first = curStackItem.getFirst(); // get the new 1st item
		    } // for all items with same label

  		    if( addedTriggerPosition ) {
  			nextStackItem.addItem( new PatternTracer( patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0 ) );
  		    }
		} while( !foundFinal && ! nextStackItem.isGood() && curStackItem.isGood() );

 		if( nextStackItem.isGood() ) stack_.forward();
 		else if( ! curStackItem.isGood() ) stack_.back();
		
	    } while( !foundFinal && isGood() );
	    
 	    if( isGood() ) ++tokenCount_;

 	    return isGood();
	}
	
	size_t getPHValue() const {
	    return dicPHValue_;
	}

	const wchar_t* getPattern() const {
	    return patterns_.at( curError_->getPatternID() ).c_str();
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

	size_t dicPHValue_;
	const Error* curError_;
	size_t tokenCount_;
	bool isGood_;

    }; // class PatternApplier


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
// 	    std::wcout<<*it<<std::endl;
	    patternGraph_.addToken( it->c_str(), 0 );
	}
	patternGraph_.finishConstruction();

	patternGraph_.writeToFile( "./patterns.mdic" );
    }


} // eon

#include "./Position.tcc"

    
    
    
