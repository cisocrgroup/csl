#ifndef CSL_MSMATCH_H
#define CSL_MSMATCH_H CSL_MSMATCH_H

#include "../LevFilter/LevFilter.h"
#include "../Trie/Trie.h"
#include "../Alphabet/Alphabet.h"
#include "../LevDEA/LevDEA.h"
#include "../Global.h"

namespace csl {
    /**
     * MSMatch provides approximate matching in large dictionaries using determinstic Levenshtein automata.
     * It is an implementation of the concepts described in
     * "S. Mihov and K. Schulz. Fast approximate search in large dictionaries. Computational Linguistics, 30, 2004."
     * Basically the implementation is derived from a C-implementation of Stoyan Mihov.
     *
     * It uses the universal levenshtein automaton that is implemented in class LevDEA.
     * @see csl::LevDEA
     *
     * @author Uli Reffle, <uli@reffle.de>
     * @date   2006
     */
    template< MSMatchMode Mode = STANDARD >
    class MSMatch : public LevFilter {
    private:
	typedef Trie Dictionary;
	const Alphabet& alph_;
	Dictionary dictFW_;
	Dictionary dictBW_;
	LevDEA* levDEAs_[3];

	LevDEA* curLevDEA_;
	Dictionary* curDict_;
	bool rev_;

	size_t k_;

	static const size_t MAX_STACKSIZE = Global::lengthOfWord + 50;

	const uchar pattern_[Global::lengthOfWord]; ///< the currently used pattern
	const uchar patLeft_[Global::lengthOfWord];
	const uchar patRight_[Global::lengthOfWord];
	const uchar patLeftRev_[Global::lengthOfWord];
	const uchar patRightRev_[Global::lengthOfWord];

	uchar word[Global::lengthOfWord]; ///< the word that is currently constructed
	ResultSet_if* output_; ///< the currently used output container

	void query_rec(int dicPos, LevDEA::Pos levPos, int depth);

    public:
	MSMatch(const Alphabet& init_alphabet, size_t k, char* compdicFile, char* compdicRevFile = 0 );
	~MSMatch();
	int query(const uchar* pattern, ResultSet_if& output);
	inline size_t maxResults() const {return MAX_RESULTS;}

	static const size_t MAX_RESULTS = 1000; 
    };

} // eon

#include "./MSMatch.cxx"
#endif
