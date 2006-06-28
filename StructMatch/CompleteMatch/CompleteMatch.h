#ifndef COMPLETEMATCH_H
#define COMPLETEMATCH_H COMPLETEMATCH_H

#include<iostream>
#include "../../LevFilter/LevFilter.h"
#include "../../Trie/Trie.h"
#include "../../Alphabet/Alphabet.h"
#include "../../LevDEA/LevDEA.h"

namespace csl {

    class CompleteMatch {
    private:
	Alphabet& alph_;
	Trie db_;

	LevDEA* ld_[Global::Perm::maxNrOfTokens];
	LevDEA& ld(int i) {
	    return *(ld_[i]);
	}

	int results_[Global::Perm::maxNrOfResults];
	int nrOfResults_;

	int nrOfTokens_;

	void query_rec(int dbPos, int token, LevDEA::Pos levPos);

	int tokenDelim;
	int compDelim;


    public:
	CompleteMatch(Alphabet& alph, char* db_file, int dlev);

	/**
	   performs the search and returns the number of found results.
	   The results can be fetched using the getResults-method
	   @param query The query-string
	   @returns the number of results found
	*/
	int query(uchar* query);

	const int* getResults() {
	    return results_;
	}


    };
} // eon;

#endif
