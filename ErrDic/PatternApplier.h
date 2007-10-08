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
		
	    inline Position( StateId_t state, const wchar_t* nextChar, bool hasError );

	    inline virtual ~Position() {}

	    inline bool isPosition() const {
		return true;
	    }

	    inline bool isPatternTracer() const {
		return false;
	    }

	    virtual StateId_t getState() const;

	    inline void set(  StateId_t state, const wchar_t* nextChar, bool hasError );

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
	    inline PatternTracer() :
		state_( 0 ),
		nextChar_( 0 ),
		depth_( 0 ) {
	    }

	    inline PatternTracer( StateId_t state, const wchar_t* nextChar, size_t depth ) :
		state_( state ),
		nextChar_( nextChar),
		depth_( depth ){
		if( *nextChar_ == PatternApplier::patternDelimiter_ ) {
		    ++nextChar_;
		}
	    }

	    inline ~PatternTracer() {}

	    void set(  StateId_t state, const wchar_t* nextChar, size_t depth ) {
		state_ = state;
		nextChar_ = nextChar;
		depth_ = depth;
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

	private:
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    size_t depth_;
	}; // class PatternTracer



	class StackItem {
	public:
	    StackItem() :
		list_( 0 ),
		positionStore_( 0 ),
		patTracerStore_( 0 )
		{
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
		    if( listPos->isPosition() ) {
			positionToStore( (Position*)listPos );
		    }
		    else {
			patTracerToStore( (PatternTracer*)listPos );
		    }
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

	    bool empty() const {
		return list_ == 0;
	    }

	    bool isGood() const {
		return ( getFirst() && ( getFirst()->getNextChar() != 0 ) );
	    }

	    Position* newPosition( StateId_t state, const wchar_t* nextChar, bool hasError ) {
		if( positionStore_ ==  0 ) {
		    return new Position( state, nextChar, hasError );
		}
		Position* pos = positionStore_;
		positionStore_ = (Position*)pos->getNextItem();
		pos->set( state, nextChar, hasError );
		pos->setNextItem( 0 );
		return pos;
	    }
	    void positionToStore( Position* pos ) {
		pos->setNextItem( positionStore_ );
		positionStore_ = pos;		
	    }

	    PatternTracer* newPatTracer( StateId_t state, const wchar_t* nextChar, size_t depth ) {
		if( patTracerStore_ ==  0 ) {
		    return new PatternTracer( state, nextChar, depth );
		}
		PatternTracer* patTracer = patTracerStore_;
		patTracerStore_ = (PatternTracer*)patTracer->getNextItem();
		patTracer->set( state, nextChar, depth );
		patTracer->setNextItem( 0 );
		return patTracer;
	    }
	    void patTracerToStore( PatternTracer* patTracer ) {
		patTracer->setNextItem( patTracerStore_ );
		patTracerStore_ = patTracer;		
	    }

	private:
	    ListItem* list_;

	    Position* positionStore_;
	    PatternTracer* patTracerStore_;

	}; // class StackItem


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
	    stack_.at( 0 ).addItem( stack_.at( 0 ).newPosition( st, suso, false ) );
	    stack_.at( 0 ).addItem( stack_.at( 0 ).newPatTracer( patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0 ) );

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
 		    while( first && first->getNextChar() == label ) {
			Position* pos;
			PatternTracer* patTracer;
 			if( ( pos = dynamic_cast< Position* >( first ) ) ) {
 			    StateId_t nextState = dic_.walk( pos->getState(), label );
 			    Position* newPos = nextStackItem.newPosition( nextState, dic_.getSusoString( nextState ), pos->hasError() );
			    nextStackItem.addItem( newPos );
			    // The 2nd constraint has to be dropped if insertions are allowed
			    if( ! newPos->hasError() && ( newPos->getNextChar() != 0 ) ) addedTriggerPosition = true;
			    
			    if( newPos->hasError() &&
				dic_.isFinal( nextState ) && 
				! filterDic_.lookup( stack_.getWord().c_str(), 0 ) ) {
				foundFinal = true;
			    }
			    
			    pos->stepToNextChar();
			}
 			else if( ( patTracer = dynamic_cast< PatternTracer* >( first ) ) ) {
 			    StateId_t nextState = patternGraph_.walk( patTracer->getState(), label );
 			    PatternTracer* newTracer = nextStackItem.newPatTracer( nextState, patternGraph_.getSusoString( nextState ), patTracer->getDepth() + 1 );
			    // if pattern-right-side read completely
			    StateId_t patStartState;
 			    if( ( patStartState = patternGraph_.walk( nextState, patternDelimiter_ ) ) ) {
				
				typedef std::pair< StateId_t, StateId_t > TwoStates;
				std::stack< TwoStates > stack;
				
				
				ListItem* item = stack_.at( getCurDepth() - patTracer->getDepth() ).getFirst();
				while( item ) {
				    if( item->isPosition() &&
					! ((Position*)item)->hasError()
					) {
					stack.push( TwoStates( patStartState, item->getState() ) );
				    }
				    item = item->getNextItem();
				}

				while( ! stack.empty() ) {
				    StateId_t patState = stack.top().first;
				    StateId_t dicState = stack.top().second;
				    StateId_t newDicState = 0;
				    stack.pop();
				    for( const wchar_t* c = patternGraph_.getSusoString( patState );
					 *c != 0; ++c ) {
					if( ( newDicState = dic_.walk( dicState, *c ) ) ) {
					    StateId_t newPatState = patternGraph_.walk( patState, *c );
					    if( patternGraph_.isFinal( newPatState ) ) {
						Position* newPos = nextStackItem.newPosition( newDicState, dic_.getSusoString( newDicState ), true );
						nextStackItem.addItem( newPos );
						// handle addedTriggerPosition here if allowing more than one error
						if( dic_.isFinal( newDicState ) ) {
						    foundFinal = true;
						}
					    }

					    // push new pair only if newPatState has some outgoing transitions
					    if( *( patternGraph_.getSusoString( newPatState ) ) ) {
						stack.push( TwoStates( newPatState, newDicState ) );
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
  			nextStackItem.addItem( nextStackItem.newPatTracer( patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0 ) );
  		    }
		} while( !foundFinal && ! nextStackItem.isGood() && curStackItem.isGood() );

 		if( nextStackItem.isGood() ) stack_.forward();
 		else if( ! curStackItem.isGood() ) stack_.back();
		
	    } while( !foundFinal && isGood() );
	    
 	    if( isGood() ) ++tokenCount_;

 	    return isGood();
	}
	
	static const wchar_t patternDelimiter_ = L',';

    private:
	size_t getCurDepth() const {
	    return stack_.getDepth();
	}


	Stack stack_;
	const MinDic< int >& dic_;
	const MinDic< int >& filterDic_;
	
	MinDic< int > patternGraph_;

	size_t tokenCount_;
	bool isGood_;

    }; // class PatternApplier


    void PatternApplier::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	
	std::vector< std::wstring > patterns;
	std::wstring line;
	while( getline( fi, line ) ) {
	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( "ErrDicConstructor: Invalid line in pattern file" );
	    }
	    patterns.push_back( line.substr( delimPos + 1 ) + patternDelimiter_ + line.substr( 0, delimPos ) );
	    
	    
	}
	fi.close();
	std::sort( patterns.begin(), patterns.end() );
	patternGraph_.initConstruction();
	for( std::vector< std::wstring >::const_iterator it = patterns.begin(); it != patterns.end(); ++it ) {
// 	    std::wcout<<*it<<std::endl;
	    patternGraph_.addToken( it->c_str(), 0 );
	}
	patternGraph_.finishConstruction();

	patternGraph_.writeToFile( "./patterns.mdic" );
    }


} // eon

#include "./Position.tcc"

    
    
    