#include<iostream>
#include<fstream>
#include<map>
#include<queue>
#include<string>
#include<vector>
#include "../Global.h"
#include "../Getopt/Getopt.h"
namespace csl {

    /**
     * A specialised variant of the Aho-Corasick trie.
     */
    class PatternGraph {
    public:
	typedef std::vector< std::wstring > RightSides_t;

	class State {
	public:
	    typedef std::vector< std::pair< wchar_t, State > >::const_iterator iterator;  
	    State( const PatternGraph& myGraph, size_t stateIndex ) :
		myGraph_( &myGraph ),
		stateIndex_( stateIndex ) {
	    }
	    
	    /**
	     * walk in the AC-Trie. Use error links when needed. 
	     *
	     * @return the "distance" from the root to the target state
	     */
	    inline bool walk( wchar_t c );

	    inline bool walkErrorLink();
	    
	    /**
	     * walk in the trie only - do not use error links
	     * if walk is not possible, stay where you are
	     *
	     * @return true if an appropriate transition was found
	     */
	    inline bool walkStrict( wchar_t c );

	    /**
	     * This is equivalent to a 'walk' call, only that the state itself
	     * rmains unchanged (-->const), but a copy of the target state
	     */
	    inline State getTransTarget( wchar_t c ) const;

	    /**
	     * @return an iterator positioned at the first of the state's transitions
	     */
	    inline iterator begin() const;

	    /**
	     * @return a past-the-end iterator for the state's transitions
	     */
	    inline iterator end() const;

	    /**
	     * @return the "distance" from the root to the target state
	     */
	    inline size_t getDepth() const;
	    
	    inline bool isFinal() const;

	    /**
	     * @return a reference to the list of rightSides attached to the state (which must be final)
	     */
	    inline const RightSides_t& getRightSides() const;

	    inline const std::wstring& getWord() const;

	    /**
	     * @return true iff the current state is not the failstate
	     */
	    inline bool isValid() const;

	    inline void addTransition( wchar_t c, const State& target );
	    size_t getStateIndex() const;

	private:
	    friend class PatternGraph;
	    friend class InternalState;
	    
	    const PatternGraph* myGraph_;
	    size_t stateIndex_;
	    
	}; // class State

	
	class InternalState {
	public:
	    InternalState() :
		errorLink_( 0 ), depth_( 0 ), isFinal_( false ) {
	    }
	    
	    /**
	     * Returns the target stateIndex when walking with char c (Only in the trie, no errorlinks!). 
	     *
	     * @return the stateIndex of the follow-up state when walking in the trie with char c. ( 0 for fail state)
	     */
	    inline size_t getTransTarget( wchar_t c ) const {
		std::vector< Transition_t >::const_iterator transition = transitions_.begin();
		while( transition != transitions_.end() &&
		       transition->first != c ) {
		    ++transition;
		}
		if( transition != transitions_.end() ) {
		    return transition->second.getStateIndex();
		}
		else {
		    return 0;
		}
	    }
	    
	    typedef std::pair< wchar_t, State > Transition_t;
	    std::vector< Transition_t > transitions_;
	    RightSides_t rightSides_;
	    size_t errorLink_;
	    std::wstring word_;
	    int depth_;
	    bool isFinal_;
	}; // class InternalState
	

	// *** PRIVATE of PATTERNGRAPH *** 
	
	inline void loadPatterns( const char* patternFile );

	inline void addErrorLinks();

	/**
	 * Add a new state and return its index in the vector
	 */
	inline State newState();

	inline State getRoot() const;

	inline void toDot() const;


	/**
	 * Don't forget the realloc-trouble here !!!
	 */
	std::vector< InternalState > states_;

	static const wchar_t patternDelimiter_ = L'_';
    };


    /********** IMPL   STATE  ********************/

    bool PatternGraph::State::walk( wchar_t c ) {
//	std::wcout<<"Reading "<<c<<", beginning at state "<<getStateIndex()<<std::endl; // DEBUG
	while( isValid() && ( ! walkStrict( c ) ) ) {
	    stateIndex_ = myGraph_->states_.at( getStateIndex() ).errorLink_;
	    // std::wcout<<"  Retreat to "<<getStateIndex()<<std::endl; // DEBUG
	}
	if( ! isValid() ) {
	    stateIndex_ = 1;
	}
	return isValid();
    }

    bool PatternGraph::State::walkStrict( wchar_t c ) {
	if( size_t newIndex =  myGraph_->states_.at( stateIndex_ ).getTransTarget( c ) ) {
	    stateIndex_ = newIndex;
	    return true;
	}
	else {
	    return false;
	}
    }

    bool PatternGraph::State::walkErrorLink() {
	stateIndex_ = myGraph_->states_.at( getStateIndex() ).errorLink_;
	return isValid();
    }

    inline PatternGraph::State PatternGraph::State::getTransTarget( wchar_t c ) const {
	State newState = *this;
	newState.walk( c);
	return newState;
    }

    inline size_t PatternGraph::State::getDepth() const {
	return myGraph_->states_.at( stateIndex_ ).depth_;
    }

    inline PatternGraph::State PatternGraph::getRoot() const {
	return State( *this, 1 );
    }

    inline size_t PatternGraph::State::getStateIndex() const {
	return stateIndex_;
    }

    inline PatternGraph::State::iterator PatternGraph::State::begin() const {
	return myGraph_->states_.at( stateIndex_ ).transitions_.begin();
    }

    inline PatternGraph::State::iterator PatternGraph::State::end() const {
	return myGraph_->states_.at( stateIndex_ ).transitions_.end();
    }
    
    inline bool PatternGraph::State::isFinal() const {
	return myGraph_->states_.at( stateIndex_ ).isFinal_;
    }

    inline bool PatternGraph::State::isValid() const {
	return ( stateIndex_ != 0 );
    }

    inline const PatternGraph::RightSides_t& PatternGraph::State::getRightSides() const {
	return myGraph_->states_.at( stateIndex_ ).rightSides_;
    }

    inline const std::wstring& PatternGraph::State::getWord() const {
	return myGraph_->states_.at( stateIndex_ ).word_;
    }


    /********* IMPL  PATTERNGRAPH *****************/

    inline void PatternGraph::loadPatterns( const char* patternFile ) {
	states_.resize( 2 ); // failure state at position 0, root at position 1 

	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	if( ! fi ) {
	    throw csl::exceptions::badFileHandle( "PatternGraph::Could not open pattern file" );
	}
	
	std::wstring line;

	size_t patternCount = 0;
	while( getline( fi, line ) ) {
	    size_t delimPos = line.find( L' ' );
	    if( delimPos == std::wstring::npos ) {
		throw csl::exceptions::badInput( "PatternGraph: Invalid line in pattern file" );
	    }
 	    std::wstring left = line.substr( 0, delimPos );
 	    std::wstring right = line.substr( delimPos + 1 );

	    State state = getRoot();

	    // *** find common prefix ***
	    std::wstring::const_iterator c = left.begin();
	    for( ;
		 c != left.end() && state.walkStrict( *c );
		 ++c ) {
		// do nothing else
	    }
	    
	    // *** add suffix ***
	    // prevent the vector from re-allocating during the process
	    states_.reserve( states_.size() + left.end() - c );
	    
	    State lastState = state;
	    for( ;
		 c != left.end();
		 ++c ) {
		State newSt = newState();
		states_.at( newSt.getStateIndex() ).depth_ = c - left.begin() + 1;
		states_.at( lastState.getStateIndex() ).transitions_.push_back( std::make_pair( *c, newSt ) );
		lastState = newSt;
	    }
	    states_.at( lastState.getStateIndex() ).isFinal_ = true;
	    states_.at( lastState.getStateIndex() ).rightSides_.push_back( right );
	    states_.at( lastState.getStateIndex() ).word_ = left;
	}
	fi.close();

	addErrorLinks();

	// toDot(); // DEBUG

    }

    inline void PatternGraph::addErrorLinks() {

	states_.at( 1 ).errorLink_ = 0;
	std::queue< size_t > queue;
	queue.push( getRoot().getStateIndex() );

	while( ! queue.empty() ) {
	    size_t parent = queue.front();
	    queue.pop();

	    for( std::vector< std::pair< wchar_t, State > >::const_iterator transition = states_.at( parent ).transitions_.begin();
		 transition != states_.at( parent ).transitions_.end();
		 ++transition ) {
		queue.push( transition->second.getStateIndex() );
		
		wchar_t label = transition->first;
		size_t current = transition->second.getStateIndex();
		size_t parentBack = parent;
		size_t back = 0;
		
		while( ( back == 0 ) && ( parentBack != 0 ) ) {
		    parentBack = states_.at( parentBack ).errorLink_;
		    back = states_.at( parentBack ).getTransTarget( label );
		}
		if( parentBack == 0 ) back = 1;

		if( states_.at( back ).isFinal_ ) {
		    states_.at( current ).isFinal_ = true;
		}
		states_.at( current).errorLink_ = back;
	    }
	}
    } // method PatternGraph::addErrorLinks

    inline PatternGraph::State PatternGraph::newState() {
	states_.resize( states_.size() + 1 );
	return State( *this, states_.size() -1 );
    }

    inline void PatternGraph::toDot() const {
	printf( "Digraph PatternGraph { //DOTCODE\nrankdir=LR; //DOTCODE\nordering=out; //DOTCODE\n" );

	size_t count = 0;
	for( std::vector< InternalState >::const_iterator st = states_.begin() ; st != states_.end(); ++st ) {
	    std::wcout<<count<<"[label=\""<<count<<"\",peripheries="<< ( (st->isFinal_)? "2" : "1" ) << "] // DOTCODE" << std::endl;
	    for( std::vector< std::pair< wchar_t, State > >::const_iterator trans = st->transitions_.begin();
		 trans != st->transitions_.end();
		 ++trans ) {
		
		std::wcout << count << "->" << trans->second.getStateIndex() << "[label=\"" << trans->first <<"\"] // DOTCODE" << std::endl;
	    }
	    std::wcout << count << "->" << st->errorLink_ << "[color=red, constraint=false] // DOTCODE" << std::endl;
	

	    ++count;
	}

	std::wcout << "} // DOTCODE" << std::endl;
    }


} // namespace csl
