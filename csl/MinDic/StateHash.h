

/**
 * Statehash is a hashtable to store states of a MinDic during dictionary construction.
 * This allows an efficient check if a new state is equivalent to an existing one.
 */
class StateHash {
public:

private:
    TransTable_t& trans_;

    class ChainLink {
    public:
	StateId_t value;
	ChainLink* next;
	ChainLink() {
	    value = 0;
	    next = NULL;
	}
    };

    ChainLink** table_;

    static const int HASHC1 = 257;
//    static const int HASHC2 = 107;

    static const size_t tableSize_ = (size_t)( 1ll<<25 ) - 1;

    template< typename TempState >
    int hashcode(const TempState& state) {
                int h = (state.isFinal())? 0 : Global::maxNrOfChars;
        h += state.getAnnotation();

//      for(int j = 1; j <= alphSize_; h = h*HASHC1 + j + alphSize_ * state.getTransTarget(j), ++j);
        for( typename TempState::const_TransIterator it = state.transBegin();
             it != state.transEnd();
			 ++it ) {
			h = h*HASHC1 + it->getLabel() + Global::maxNrOfChars * it->getTarget();
		}
                return (abs(h) % tableSize_);
    }
    
public:
    StateHash(TransTable_t& trans) : trans_(trans) {
        try {
            table_ = new ChainLink*[tableSize_];
	    memset( table_, 0, tableSize_ * sizeof(ChainLink*) );
        } catch( std::bad_alloc exc ) {
            std::cout<<"csl::StateHash: Could not allocate hashtable: " <<  exc.what() << std::endl;
            throw exc;
	}
    }
    
    ~StateHash() {
 	ChainLink* cur = 0;
	ChainLink* next = 0; 
 	for( size_t i=0; i < tableSize_ ; ++i ) {
 	    cur = table_[i];
 	    while( cur != 0 ) {
 		//std::wcout<< "i=" <<i << ", cur="<< cur << ", next="<< cur->next << std::endl;
 		next = (*cur).next;
		delete( cur );
		cur = next;
 	    }
      }
    }

    template< typename TempState >
    void push( const TempState& state, StateId_t compId ) {
        size_t slot = hashcode( state );
        ChainLink* newCL = new ChainLink();
	(*newCL).value = compId;
	(*newCL).next = table_[slot];
        table_[slot] = newCL;
    }

    template< typename TempState >
    size_t findState(const TempState& state) {
                ChainLink* ch = table_[hashcode(state)];
                while( ch && ! trans_.compareStates( state, ch->value ) ) {
                        ch = ch->next;
		}
		return ( ch )? ch->value : 0;
    }
};
