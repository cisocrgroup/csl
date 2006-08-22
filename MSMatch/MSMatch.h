#ifndef CSL_MSMATCH_H
#define CSL_MSMATCH_H CSL_MSMATCH_H

#include "../LevFilter/LevFilter.h"
#include "../Trie/Trie.h"
#include "../Alphabet/Alphabet.h"
#include "../LevDEA/LevDEA.h"
#include "../Global.h"

namespace csl {
/**
   MSMatch provides approximate matching in large dictionaries. It is an implementation of the concepts described in
   "S. Mihov and K. Schulz. Fast approximate search in large dictionaries. Computational Linguistics, 30, 2004."
   Basically the implementation is derived from a C-implementation of Stoyan Mihov.

   It uses the universal levenshtein automaton that is implemented in class LevDEA.

   @author Uli Reffle, <uli@reffle.de>
   @date   2006
*/
    template< MSMatchMode Mode = STANDARD >
    class MSMatch : public LevFilter {
    private:
	const Alphabet& alph_;
	Trie dic;
	LevDEA levDEA2;
	LevDEA levDEA;
	static const int MAX_STACKSIZE = Global::lengthOfWord + 50;

	const uchar* pattern_; ///< the currently used pattern
	uchar word[Global::lengthOfWord]; ///< the word that is currently constructed
	ResultSet_if* output_; ///< the currently used output container

	void query_rec(int dicPos, LevDEA::Pos levPos, int depth);

    public:
	MSMatch(const Alphabet& init_alphabet,char* compdic_file, int lev_distance);   
	~MSMatch();
	int query(const uchar* pattern, ResultSet_if& output);
	inline int maxResults() const {return MAX_RESULTS;}

	static const int MAX_RESULTS = 1000; 
    };

} // eon

#include "./MSMatch.cxx"
#endif
