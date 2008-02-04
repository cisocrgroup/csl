
namespace csl {

    /**
     * A specialised variant of the Aho-Corasick trie.
     */
    class PatternGraph {
    public:
	class State {
	public:
	    /**
	     * walk in the AC-Trie. Use error links when needed. 
	     *
	     * @return the "distance" from the root to the target state
	     */
	    inline size_t walk( wchar_t c ) const;

	    /**
	     * walk in the trie only - do not use error links
	     * if walk is not possible, stay where you are
	     *
	     * @return true if an appropriate transition was found
	     */
	    inline bool walkStrict( wchar_t c ) const;

	    /**
	     * @return the "distance" from the root to the target state
	     */
	    inline const State& getTransTarget( wchar_t c ) const;

	    inline size_t getDepth() const;
	    
	    inline bool isFinal() const;

	    inline void addTransition( wchar_t c, const State& target );

	private:
	    PatternGraph* myGraph_;
	    size_t stateIndex_;
	    
	};

	class InternalState {
	public:
	    friend class State;
	private:
	    std::vector< std::pair< wchar_t, size_t > > transitions_;
	    size_t errorLink_;
	    int depth_;
	    bool isFinal_;

	};
	
	/**
	 * Don't forget the realloc-trouble here !!!
	 */
	std::vector< InternalState > states_;

	inline void loadPatterns( const char* patternFile );

	inline const State& getRoot() const;

	inline void toDot() const;


    };


    size_t PatternGraph::State::walk( wchar_t c ) const {
	return 42;
    }

    bool PatternGraph::State::walkStrict( wchar_t c ) const {
	return 42;
    }

    inline const PatternGraph::State& PatternGraph::State::getTransTarget( wchar_t c ) const {
	return *this;
    }

    inline int PatternGraph::State::getDepth() const {
	return depth_;
    }

    inline void PatternGraph::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );
	if( ! fi ) {
	    throw csl::exceptions::badFileHandle( "PatternGraph::Could not open pattern file" );
	}
	
	typedef std::map< std::wstring, RightList_t > PatternMap_t;
	PatternMap_t patterns;
	std::wstring line;

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
		State& 
		states_.resize( states_.size() + 1 );
		
		
	    }

	    patterns[left].push_back( right );
	}
	fi.close();

	leftSides_.initConstruction();

	
	for( PatternMap_t::const_iterator it = patterns.begin(); it != patterns.end(); ++it ) {

// 	    std::wcout<<it->first<<"->";
// 	    for( RightList_t::const_iterator rit = it->second.begin(); rit != it->second.end(); ++rit ) {
// 		std::wcout<<*rit<<" | ";
// 	    }
// 	    std::wcout<<std::endl;

	    // here we copy the whole vector, but who cares ....
	    rightSides_.push_back( it->second );

	    // this might be done better :-)
	    std::wstring reReversed;
	    Global::reverse( it->first, &reReversed );
	    
	    leftSidesList_.push_back( reReversed );
	    leftSides_.addToken( it->first.c_str(), rightSides_.size()-1 ); // annotate the proper index into rightSides_
	}
	leftSides_.finishConstruction();
//	leftSides_.toDot(); // DEBUG
	
    }

    inline void PatternGraph::toDot() const {
	
    }


} // namespace csl
