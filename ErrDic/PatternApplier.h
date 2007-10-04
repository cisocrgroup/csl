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
		if( getNextChar() == 0 ) return false; // sort those objects to the very end

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

	    inline void set(  StateId_t state, const wchar_t* curChar, bool hasError );

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
	    inline PatternTracer( StateId_t state, const wchar_t* nextChar, size_t depth ) :
		state_( state ),
		nextChar_( nextChar),
		depth_( depth ){
	    }

	    inline ~PatternTracer() {}

    
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
		list_( 0 )
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
		    delete( listPos );
		    listPos = nextPos;
		}
		list_ = 0;
		
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
	PatternApplier( const MinDic< int >& dic, const MinDic< int >& filterDic, const char* patternFile ) :
	    dic_( dic ),
	    filterDic_( filterDic ),
	    isGood_( false ){
	    
	    loadPatterns( patternFile );

	    
	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    // insert initial positions and patTracers
	    stack_.at( 0 ).addItem( new Position( st, suso, false ) );
	    stack_.at( 0 ).addItem( new PatternTracer( patternGraph_.getRoot(), patternGraph_.getSusoString( patternGraph_.getRoot() ), 0 ) );

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
		    std::wcout<<"word: "<<stack_.getWord()<<std::endl;
		    
		    bool addedPosition = false;
 		    while( first && first->getNextChar() == label ) {
			Position* pos;
			PatternTracer* patTracer;
 			if( ( pos = dynamic_cast< Position* >( first ) ) ) {
 			    StateId_t nextState = dic_.walk( pos->getState(), label );
 			    Position* newPos = new Position( nextState, dic_.getSusoString( nextState ), pos->hasError() );
			    nextStackItem.addItem( newPos );
			    addedPosition = true;
			    
			    if( newPos->hasError() &&
				dic_.isFinal( nextState ) && 
				! filterDic_.isFinal( nextStackItem.getFilterPos() ) ) {
				foundFinal = true;
			    }
			    
			    pos->stepToNextChar();
			}
 			else if( ( patTracer = dynamic_cast< PatternTracer* >( first ) ) ) {
 			    StateId_t nextState = patternGraph_.walk( patTracer->getState(), label );
 			    PatternTracer* newTracer = new PatternTracer( nextState, dic_.getSusoString( nextState ), patTracer->getDepth() + 1 );
			    // if pattern read completely
 			    if( StateId_t patStartState = patternGraph_.walk( nextState, L' ' ) ) {
				typedef std::pair< StateId_t, StateId_t > TwoStates;
				std::stack< TwoStates > stack;
				
				
				ListItem* item = stack_.at( getCurDepth() - patTracer->getDepth() ).getFirst();
				while( item ) {
				    if( item->isPosition() ) {
					stack.push( TwoStates( patStartState, item->getState() ) );
				    }
				    item = item->getNextItem();
				}

				while( ! stack.empty() ) {
				    StateId_t patState = stack.top().first;
				    StateId_t dicState = stack.top().second;
				    StateId_t newDicState = 0;
				    if( patternGraph_.isFinal( patState ) ) {
					Position* newPos = new Position( dicState, dic_.getSusoString( dicState ), true );
					nextStackItem.addItem( newPos );
					addedPosition = true;
					
					if( dic_.isFinal( dicState ) ) {
					    foundFinal = true;
					}
				    }
				    stack.pop();
				    for( const wchar_t* c = patternGraph_.getSusoString( patState );
					 *c != 0; ++c ) {
					if( ( newDicState = dic_.walk( dicState, *c ) ) ) {
					    stack.push( TwoStates( patternGraph_.walk( patState, *c ), newDicState ) );
					}
				    }
				}
				
 			    }
				
			    nextStackItem.addItem( newTracer );
			    patTracer->stepToNextChar();
				
			}
			
			curStackItem.arrangeFirst(); // re-arrange list of ListItems
			first = curStackItem.getFirst(); // get the new 1st item
		    } // for all items with same label

//  		    if( nextStackItem.getNewStates().size() > 0 ) {
//  			nextStackItem.addItem( new PatternTracer( patternTo_.c_str() ) );
//  		    }
		} while( !foundFinal && ! nextStackItem.isGood() && curStackItem.isGood() );

 		if( ! nextStackItem.isGood() ) stack_.back();
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
		size_t delimPos = line.find( ' ' );
		if( delimPos == std::wstring::npos ) {
		    throw exceptions::badInput( "ErrDicConstructor: Invalid line in pattern file" );
		}
		patterns.push_back( line.substr( delimPos + 1 ) + L' ' + line.substr( 0, delimPos ) );
	    
	    
	    }
	    fi.close();
	    std::sort( patterns.begin(), patterns.end() );
	    patternGraph_.initConstruction();
	    for( std::vector< std::wstring >::const_iterator it = patterns.begin(); it != patterns.end(); ++it ) {
// 	    std::wcout<<*it<<std::endl;
		patternGraph_.addToken( it->c_str(), 0 );
	    }
	    patternGraph_.finishConstruction();

	}


    } // eon

#include "./Position.tcc"

    
    
    
