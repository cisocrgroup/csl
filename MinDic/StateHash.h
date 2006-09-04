
/**
 * Statehash is a hashtable to store states of a MinDic during dictionary construction.
 * This allows an efficient check if a new state is equivalent to an existing one.
 */
class StateHash {
 private:
    TransTable_t& trans_;
    int alphSize_;

    class ChainLink {
    public:
	int value;
	ChainLink* next;
	ChainLink() {
	    value = 0;
	    next = NULL;
	}
    };

    ChainLink** table_;

    static const int HASHC1 = 257;
    static const int HASHC2 = 107;

    static const int tableSize_ = (1<<20) -1;

    int hashcode(const TempState_t& state) {
	int h = (state.isFinal())? 0 : alphSize_;
	for(int j=1; j <= alphSize_; h = h*HASHC1 + j + alphSize_ * state.getTransTarget(j), ++j);
	
	return (abs(h) % tableSize_);
    }


 public:
    StateHash(TransTable_t& trans) : trans_(trans),alphSize_(trans_.getAlphSize()) {
	table_ = new ChainLink*[tableSize_];
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

    void push(const TempState_t& state, int compId) {
	int slot = hashcode(state);
	ChainLink* newCL = new ChainLink();
	(*newCL).value = compId;
	(*newCL).next = table_[slot];
	table_[slot] = newCL;
    }

    int findState(const TempState_t& state) {
	ChainLink* ch = table_[hashcode(state)];
	while(ch && !trans_.compareStates(state,ch->value)) {
	    ch = ch->next;
	}
	return (ch)? ch->value : 0;
    }
};
