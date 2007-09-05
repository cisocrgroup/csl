#ifndef CSL_MSMATCH_H
#define CSL_MSMATCH_H CSL_MSMATCH_H

#include<string>
#include<map>
#include "../LevFilter/LevFilter.h"
#include "../MinDic/MinDic.h"
#include "../FBDic/FBDic.h"
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
    public:
	/**
	 * Three modes are offered for case handling:
	 * 
	 */
	enum CaseMode { asIs, // don't change anything, take query as it comes
			toLower, // change query to lower case and return all candidates lower case 
			restoreCase // do search in lower case, but upcase first character of all cands if query was uppercase 
	}; 

    private:
	typedef MinDic< int > MinDic_t;
	FBDic< int > fbDic_;
	const MinDic_t& dictFW_;
	const MinDic_t& dictBW_;

	LevDEA* levDEAFirst_;
	LevDEA* levDEASecond_;

	CaseMode caseMode_;

	LevDEA* curLevDEA_; ///< used for the STANDARD algorithm
	LevDEA* curLevDEAFirst_; ///< used for the FW_BW algorithm
	LevDEA* curLevDEASecond_; ///< used for the FW_BW algorithm
	const MinDic_t* curDict_;
	bool reverse_;

	size_t k_;

	static const size_t MAX_STACKSIZE = Global::lengthOfWord + 50;

	wchar_t pattern_[Global::lengthOfWord]; ///< the currently used pattern
	wchar_t patLeft_[Global::lengthOfWord];
	wchar_t patRight_[Global::lengthOfWord];
	wchar_t patLeftRev_[Global::lengthOfWord];
	wchar_t patRightRev_[Global::lengthOfWord];

	int minDistFirst_;
	int minDistSecond_;

	wchar_t word_[Global::lengthOfWord]; ///< the word that is currently constructed
	CandidateReceiver* candReceiver_; ///< the currently used candReceiver
	// this may also be -1
	int levDistanceFirst_;

	typedef std::map< std::wstring, std::pair< size_t, int > > CandidateMap;
	CandidateMap results_;

	inline void intersect( int dicPos, LevDEA::Pos levPos, int depth );
	inline void intersectFirst( int dicPos, LevDEA::Pos levPos, int depth );
	inline void intersectSecond( int dicPos, LevDEA::Pos levPos, int depth );

	inline void queryCases_1();
	inline void queryCases_2();
	inline void queryCases_3();

    public:
	inline MSMatch( size_t k, const char* dicFile );
	~MSMatch();

	inline int query( const wchar_t* pattern, CandidateReceiver& candReceiver );

	inline const MinDic_t& getFWDic() const {
	    return dictFW_;
	}
	inline void setCaseMode( CaseMode caseMode ) {
	    caseMode_ = caseMode;
	}

  };

} // eon

#include "./MSMatch.tcc"
#endif
