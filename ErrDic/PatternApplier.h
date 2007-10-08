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
	    typedef enum{ DIC, PATTRACER } PosType;
	    inline Position();
		
	    inline Position( PosType type, StateId_t state, const wchar_t* nextChar, size_t errorsOrDepth );

	    inline virtual ~Position() {}

	    inline bool isPosition() const {
		return true;
	    }

	    inline bool isPatternTracer() const {
		return false;
	    }

	    virtual StateId_t getState() const;

	    inline void set( PosType posType, StateId_t state, const wchar_t* nextChar, size_t errorsOrDepth );

	    inline size_t hasError() const;
	    inline size_t getDepth() const;

	    inline wchar_t getNextChar() const;

	    inline bool stepToNextChar();

	    inline void setNextItem( Position* nextItem ) {
		nextItem_ = nextItem;
	    }
	    inline Position* getNextItem() const {
		return nextItem_;
	    }
	    
	    bool operator<( const Position& other ) const {
		if( getNextChar() == 0 ) {
		    return false; // sort those objects to the very end
		}
		else if( other.getNextChar() == 0 ) {
		    return true; // sort those objects to the very end
		}
		return this->getNextChar() < other.getNextChar();
	    }

	    inline PosType getType() const {
		return posType_;
	    }

	private:
	    PosType posType_;
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    size_t errorsOrDepth_;
	    Position* nextItem_;
	}; // class Position



	class StackItem {
	public:
	    StackItem() :
		list_( 0 ),
		positionStore_( 0 )
		{
		}

	    /**
	     * The destructor does NOT call clear(), that is, does NOT remove the Position-objects
	     * from the heap. That way, the StackItems can easily be copied.
	     * Call clear() yourself if you now you don't need the StackItem any more ... 
	     */
	    ~StackItem() {
	    }

	    void clear() {
		listToStore( list_ );
		list_ = 0;
	    }

	    void addItem( Position* newItem ) {
		Position* listPos = list_;
		Position* lastPos = 0;
		while( ( listPos != 0 ) && ( *listPos < *newItem ) ) {
		    lastPos = listPos;
		    listPos = listPos->getNextItem();
		    
		}
		if( lastPos ) lastPos->setNextItem( newItem );
		else list_ = newItem;
		
		newItem->setNextItem( listPos );
	    }
	    
	    Position* getFirst() {
		return list_;
	    }

	    const Position* getFirst() const {
		return list_;
	    }

	    void arrangeFirst() {
		// log out first item and add it again
		Position* first = list_;
		list_ = list_->getNextItem();

		addItem( first );
	    }

	    bool empty() const {
		return list_ == 0;
	    }

	    bool isGood() const {
		return ( getFirst() && ( getFirst()->getNextChar() != 0 ) );
	    }

	    Position* newPosition( Position::PosType posType, StateId_t state, const wchar_t* nextChar, size_t errorsOrDepth ) {
		if( positionStore_ ==  0 ) {
		    return new Position( posType, state, nextChar, errorsOrDepth );
		}
		Position* pos = positionStore_;
		positionStore_ = pos->getNextItem();
		pos->set( posType, state, nextChar, errorsOrDepth );
		pos->setNextItem( 0 );
		return pos;
	    }
	    void listToStore( Position* list ) {
		if( ! list ) return;
		Position* last = list;
		while( last->getNextItem() ) {
		    last = last->getNextItem();
		}
		last->setNextItem( positionStore_ );
		positionStore_ = list;
	    }

	private:
	    Position* list_;

	    Position* positionStore_;

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
	    stack_.at( 0 ).addItem( stack_.at( 0 ).newPosition( Position::DIC, st, suso, false ) );
	    stack_.at( 0 ).addItem( stack_.at( 0 ).newPosition( Position::PATTRACER, patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0 ) );

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


 		    Position* first = curStackItem.getFirst();
 		    wchar_t label = first->getNextChar();

		    
 		    stack_.getWord().resize( getCurDepth() + 1 );
 		    stack_.getWord().at( getCurDepth() ) = label;
		    // std::wcout<<"word: "<<stack_.getWord()<<std::endl;
		    
		    bool addedTriggerPosition = false;
 		    while( first && first->getNextChar() == label ) {
 			if( first->getType() == Position::DIC ) {
 			    StateId_t nextState = dic_.walk( first->getState(), label );
 			    Position* newPos = nextStackItem.newPosition( Position::DIC, nextState, dic_.getSusoString( nextState ), first->hasError() );
			    nextStackItem.addItem( newPos );
			    // The 2nd constraint has to be dropped if insertions are allowed
			    if( ! newPos->hasError() && ( newPos->getNextChar() != 0 ) ) addedTriggerPosition = true;
			    
			    if( newPos->hasError() &&
				dic_.isFinal( nextState ) && 
				! filterDic_.lookup( stack_.getWord().c_str(), 0 ) ) {
				foundFinal = true;
			    }
			    
			    first->stepToNextChar();
			}
 			else if( first->getType() == Position::PATTRACER ) {
 			    StateId_t nextState = patternGraph_.walk( first->getState(), label );
 			    Position* newTracer = nextStackItem.newPosition( Position::PATTRACER, nextState, patternGraph_.getSusoString( nextState ), first->getDepth() + 1 );
			    // if pattern-right-side read completely
			    StateId_t patStartState;
 			    if( ( patStartState = patternGraph_.walk( nextState, patternDelimiter_ ) ) ) {
				
				typedef std::pair< StateId_t, StateId_t > TwoStates;
				std::stack< TwoStates > stack;
				
				
				Position* item = stack_.at( getCurDepth() - first->getDepth() ).getFirst();
				while( item ) {
				    if( ( item->getType() == Position::DIC ) &&
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
						Position* newPos = nextStackItem.newPosition( Position::DIC, newDicState, dic_.getSusoString( newDicState ), 1 );
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
			    first->stepToNextChar();
				
			}
			
			curStackItem.arrangeFirst(); // re-arrange list of Positions
			first = curStackItem.getFirst(); // get the new 1st item
		    } // for all items with same label

  		    if( addedTriggerPosition ) {
  			nextStackItem.addItem( nextStackItem.newPosition( Position::PATTRACER, patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0 ) );
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

    
    
    
