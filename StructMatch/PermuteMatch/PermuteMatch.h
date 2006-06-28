#ifndef PERMUTEMATCH_H
#define PERMUTEMATCH_H PERMUTEMATCH_H

#include<cstring>
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
	MSMatch levquery_;

	Trie db_;
	List list_;

	uchar query_[Global::lengthOfStr];
	int nrOfTokens_;

	bool findParts; ///< if TRUE, report final states in db even if not all tokens of the query are processed. Default is set FALSE in the constructor
	int minParts; ///< Restricts the findParts-function to find only matches consisting of at least minParts tokens of the query

	/// a bit-vector with the nrOfTokens_ lower bits set to 1, others to 0
	bits32 cols_full;

	int countCharResults; // more or less: DEBUG!
	uchar charResults[Global::Perm::maxNrOfResults][Global::lengthOfStr]; // more or less: DEBUG!

	int countResults;
	int results[Global::Perm::maxNrOfResults];

	int tokenDelim; ///< alph-coded version of Global::Perm::tokenDelimiter
	int compDelim; ///< alph-coded version of Global::Perm::componentDelimiter


	static bool staticBitsCalculated_;
	/// b11[i] is a bitstring with the i lower bits set to 1, the others to 0
	static bits64 b11[Global::Perm::maxNrOfTokens];


	void query_rec(int db_pos,int w_pos,int list_pos, bits32 col_bits, int rmCand, int depth);
	
    public:
	PermuteMatch(Alphabet alph, char* db_file,char* dic_file,int dlev);


	/// Sets findParts. Read the doc of the variable findParts for some info
	void setFindParts(bool b) {
	    findParts = b;
	}

	/// Sets minParts. Read the doc of the variable minParts for some info
	void setMinParts(int m) {
	    if(!findParts) {
		std::cerr<<"Warning: OneListQuery: Tried to set minParts without findParts being activated"<<std::endl;
	    }
	    minParts = m;
	}


	/**
	   performs the search and returns the number of found results.
	   The results can be fetched using the getResults-method
	   @param query The query-string
	   @returns the number of results found
	*/
	int query(uchar* query);

	inline const int* getResults() {
	    return results;
	}

	void printCharResults() { //DEBUG!
	    int i;
	    for(i=0;i<countCharResults;++i) {
		std::cout<<"- "<<charResults[i]<<std::endl;
	    }

	}

	void printList() {
	    list_.printList();
	}

    };

} // eon

#endif
