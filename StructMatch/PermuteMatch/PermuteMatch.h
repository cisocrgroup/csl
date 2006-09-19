#ifndef CSL_PERMUTEMATCH_H
#define CSL_PERMUTEMATCH_H CSL_PERMUTEMATCH_H

#include <cstring>
#include <algorithm>

#include "../../Alphabet/Alphabet.h"
#include "../../MSMatch/MSMatch.h"
//#include"../BestMatch/BestMatch.h"
#include "../../Trie/Trie.h"
#include"../../Global.h"

namespace csl {

    class PermuteMatch {
    private:
#include "./List.h"
	Alphabet alph_;


	// Uncomment only one of the following two lines
	// A more comfortable way to choose between Bestmatch and Levquery would be appreciated ...
	//    BestMatch levquery;
	MSMatch< FW_BW > levquery_;

	Trie db_;
	List list_;

	uchar query_[Global::lengthOfStr];
	size_t nrOfTokens_;

	bool findParts_; ///< if TRUE, report final states in db even if not all tokens of the query are processed. Default is set FALSE in the constructor
	size_t minParts_; ///< Restricts the findParts-function to find only matches consisting of at least minParts tokens of the query

	/// a bit-vector with the nrOfTokens_ lower bits set to 1, others to 0
	bits32 cols_full;

	uchar w_[Global::lengthOfStr];
	int countCharResults; // more or less: DEBUG!
	uchar charResults[Global::Perm::maxNrOfResults][Global::lengthOfStr]; // more or less: DEBUG!

	int countResults;
	int results[Global::Perm::maxNrOfResults];

	int tokenDelimiter_; ///< alph-coded version of Global::Perm::tokenDelimiter
	int permuteDelimiter_; ///< alph-coded version of the delimiter indicating permutations to be allowed
	int noPermuteDelimiter_; ///< alph-coded version of the delimiter indicating permutations NOT to be allowed


	static bool staticBitsCalculated_;
	/// b11[i] is a bitstring with the i lower bits set to 1, the others to 0
	static bits64 b11[Global::Perm::maxNrOfTokens];


	void checkNewComponent( uint_t dbPos, // pos in the db-automaton
				size_t w_pos, // end-position of the answer string ( char-wise depth )
				size_t listPos, // pos in the candidate list
				bits32 colBits, // bitvector indicating which query tokens are already used 
				int rightmostCand, // the rightmost query token that is already used
				size_t countTokens // nr of complete tokens included in the current answer
	    );

	void findPermute( uint_t db_pos, size_t w_pos, size_t list_pos, bits32 col_bits, int rightmostCand, size_t depth );
	void findSequence( uint_t db_pos, size_t w_pos, size_t list_pos, bits32 col_bits, int rightmostCand, size_t depth );

    public:
	PermuteMatch( Alphabet alph, char* db_file, char* dic_file, char* rev_dic_file, int dlev );

	/// Sets findParts. Read the doc of the variable findParts for some info
	void setFindParts( bool b ) {
	    findParts_ = b;
	}

	/// Sets minParts. Read the doc of the variable minParts for some info
	void setMinParts( int m ) {
	    if ( !findParts_ ) {
		std::cerr << "Warning: OneListQuery: Tried to set minParts without findParts being activated" << std::endl;
	    }
	    minParts_ = m;
	}


	/**
	 * performs the search and returns the number of found results.
	 * The results can be fetched using the getResults-method
	 * @param query The query-string
	 * @returns the number of results found
	 */
	int query( uchar* query );

	inline const int* getResults() {
	    return results;
	}

	void printCharResults() { //DEBUG!
	    int i;
	    for ( i = 0;i < countCharResults;++i ) {
		std::cout << "- " << charResults[i] << std::endl;
	    }

	}

	void printList() {
	    list_.printList();
	}
	
    };

} // eon

#endif
