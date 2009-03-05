#include<iostream>
#include<fstream>
#include<map>
#include<queue>
#include<string>
#include<vector>
#include<csl/Global.h>
#include<csl/Trie/Trie.h>

#include "../Pattern/PatternSet.h"

namespace csl {

    /**
     * @brief A specialised variant of the Aho-Corasick trie to search 
     * in the set of left sides of a PatternSet.
     *
     * For the sake of simplicity and efficiency, the implementation details of
     * class \c PatternSet are used here: Internally we use simple \c size_t variables 
     * to refer to a \c Pattern instead of using the official and public \c PatternRef
     * interface. 
     *
     * @author Ulrich Reffle, 2008
     */
    class PatternGraph : public PatternSet, public Trie {
    public:
	// a string and an index into the list of patterns (from PatternSet)
	typedef std::vector< std::pair< std::wstring, size_t > > RightSides_t;
	

	inline PatternGraph();

	inline void loadPatterns( const char* patternFile );

	inline State getRoot() const;

	inline void toDot() const;


	// *** PRIVATE of PATTERNGRAPH *** 
    private:

	inline void addErrorLinks();


	Trie< int > trie_;
	std::vector< RightSides_t > allRightSides_;

	
	static const wchar_t patternDelimiter_ = L'_';
    };

    PatternGraph::PatternGraph() {
    }

    inline PatternGraph::State PatternGraph::getRoot() const {
	return State( *this, 1 );
    }


    /********* IMPL  PATTERNGRAPH *****************/

    inline void PatternGraph::loadPatterns( const char* patternFile ) {
	PatternSet::loadPatterns( patternFile );

	std::wstring curLeft;
	RightSides_t curRightSides;
	trie_.initConstruction();
	size_t leftSidesCount = 0;
	for( PatternList_t::const_iterator pattern = patternList().begin() + 1; // skip 1st pattern: it's the empty pattern!
	     pattern != patternList().end();
	     ++pattern ) {
	    //                                        -- curRightSides is empty at the first iteration of the for-loop
	    if( ( pattern->getLeft() != curLeft ) && ( curRightSides.size() > 0 ) ) {
		// NOTE: those few lines of code are duplicated after the for-loop!
		trie_.addToken( curLeft, leftSidesCount );
		allRightSides_.push_back( std::make_pair( curRightSides, pattern - patternList().begin() ); // exactly at position 'leftSidesCount'
		++leftSidesCount;
		curLeft = pattern->getLeft();
		curRightSides.clear();
		curRightSides.push_back( pattern->getRight() );
	    }
	    else {
		curRightSides.push_back( pattern->getRight() );
	    }
	    states_.at( lastState.getStateIndex() ).isFinal_ = true;
	    states_.at( lastState.getStateIndex() ).rightSides_.push_back( std::make_pair( pattern->getRight(), pattern - patternList().begin() ) );
	    states_.at( lastState.getStateIndex() ).word_ = left;
	}
	// store the very last leftSide and its rightSides
	trie_.addToken( curLeft, leftSidesCount );
	rightSides_.push_back( curRightSides ); // exactly at position 'leftSidesCount'
	++leftSidesCount;


	trie_.finishConstruction();
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

    inline void PatternGraph::toDot() const {
	std::wcout<< "Digraph PatternGraph { //DOTCODE"<<std::endl
		  <<"rankdir=LR; //DOTCODE"<<std::endl
		  <<"ordering=out; //DOTCODE"<<std::endl;
	

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
