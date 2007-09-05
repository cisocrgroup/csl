#ifdef STATEHASH_USE_NEW

namespace csl { 

    MinDic::StateHash::StateHash(TransTable_t& trans) : trans_(trans) {
	
    }

    inline void MinDic::StateHash::push(const TempState& state, int compId) {
    }

    inline int MinDic::StateHash::findState(const TempState& state) {
    }

} // eon

#endif
