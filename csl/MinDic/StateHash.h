//#define STATEHASH_USE_NEW 1


#ifdef STATEHASH_USE_NEW
class StateHash {
public:
    inline StateHash(TransTable_t& trans);
    inline void push(const TempState& state, int compId);
    inline int findState(const TempState& state);

private:
    const TransTable_t& trans_;
};


#endif



/***********************************************
 * Here is the old version with chained hashing
 ***********************************************/

#ifndef STATEHASH_USE_NEW
/**
 * Statehash is a hashtable to store states of a MinDic during dictionary construction.
 * This allows an efficient check if a new state is equivalent to an existing one.
 */
class StateHash {
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
    static const int HASHC2 = 107;

    static const size_t tableSize_ = (size_t)( 1ll<<26 ) - 1;

    int hashcode(const TempState& state) {
		int h = (state.isFinal())? 0 : Global::maxNrOfChars;
		if( state.isFinal() ) {
		}
		for( TempState::TransitionConstIterator it = state.transitionsBegin();
		     it != state.transitionsEnd();
		     h = h*HASHC1 + it->getLabel() + Global::maxNrOfChars * it->getTarget(), ++it ) {

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
/* 	ChainLink* ch, *next; */
/* 	for(int i=0;i<tableSize_;++i) { */
/* 	    next = table_[i]; */
/* 	    while((ch = next)) { */
/* 		std::cout<<"i="<<i<<", ch->next="<<ch->next<<std::endl; */
/* 		next = (*ch).next; */
/* //		delete(ch); */
/* 	    } */
/* 	} */
    }

    void push(const TempState& state, StateId_t compId) {
	size_t slot = hashcode(state);
	ChainLink* newCL = new ChainLink();
	(*newCL).value = compId;
	(*newCL).next = table_[slot];
	table_[slot] = newCL;
    }

    size_t findState(const TempState& state) {
		
		ChainLink* ch = table_[hashcode(state)];
		while( ch && !trans_.compareStates( state, ch->value ) ) {
			ch = ch->next;
		}
		return ( ch )? ch->value : 0;
    }
};
#endif
