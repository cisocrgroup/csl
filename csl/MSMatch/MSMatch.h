//sdsd
#ifndef CSL_MSMATCH_H
#define CSL_MSMATCH_H CSL_MSMATCH_H

#include<string>
#include<map>
#include<cctype>
#include "../LevFilter/LevFilter.h"
#include "../MinDic/MinDic.h"
#include "../FBDic/FBDic.h"
#include "../LevDEA/LevDEA.h"
#include "../Global.h"

namespace csl {
    /**
     * @brief MSMatch provides approximate matching in large dictionaries using determinstic Levenshtein automata.
     * It is an implementation of the concepts described in
     * "S. Mihov and K. Schulz. Fast approximate search in large dictionaries. Computational Linguistics, 30, 2004."
     * Basically the implementation is derived from a C-implementation of Stoyan Mihov.
     *
     * It uses the universal levenshtein automaton that is implemented in class LevDEA.
     *
     * MSMatch implements the interface \c LevFilter, that is, it provides a method query taking a word as input and
     * a \c CandidateReceiver as output argument. To use MSMatch, just implement a \c CandidateReceiver for your needs.<br>
     *
     * @see csl::LevDEA, csl::LevFilter, csl::LevFilter::CandidateReceiver
     *
     * @todo At the moment the levenshtein threshold can only be set once at construction. It should be easy to provide
     *       some method like setDistance() to change this anytime. 
     * @todo remove fixed-length string components. Change interface to std::wstring
     *
     * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
     * @date   2006
     */
    template< MSMatchMode Mode = STANDARD >
    class MSMatch : public LevFilter {
    public:
	/**
	 * The datatype returned by the method getFWDic().
	 */
	typedef MinDic< int > MinDic_t;

	/**
	 * Three modes are offered for case handling:
	 * 
	 */
	enum CaseMode { asIs, /**< don't change anything, take query as it comes*/
			toLower, /**< change query to lower case and return all candidates lower case */
			restoreCase /**< do search in lower case, but upcase first character of all cands if query was uppercase */
	}; 

	/**
	 * Create an MSMatch object with a levenshtein threshold \c k and a path specifying a file with a compiled FBDic.
	 * 
	 * @param k the levenshtein threshold: MSMatch will extract candidates with distance lower or equal to \c k
	 * @param FBDicFile a path specifying a file with a compiled FBDic
	 */
	inline MSMatch( size_t k, const char* FBDicFile );
	~MSMatch();

	/**
	 * @brief compute candidates for the given query patterns; store the cands in the candReceiver object.
	 * @param[in] pattern
	 * @param[out] candReceiver
	 */
	inline void query( const wchar_t* pattern, LevFilter::CandidateReceiver& candReceiver );

	/**
	 * @brief returns a reference to the used dictionary.
	 * (forward, in contrast to backward as is also used for the extraction)
	 */
	inline const MinDic_t& getFWDic() const {
	    return dictFW_;
	}

	/**
	 * @brief Specify a case mode (one of CaseMode) to decide the treatment of uppercased input.
	 * @param caseMode
	 */
	inline void setCaseMode( CaseMode caseMode ) {
	    caseMode_ = caseMode;
	}

    private:
	FBDic<> fbDic_;
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


  };

} // eon

#include "./MSMatch.tcc"
#endif
