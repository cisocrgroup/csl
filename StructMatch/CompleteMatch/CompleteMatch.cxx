#ifndef COMPLETEMATCH_CXX
#define COMPLETEMATCH_CXX COMPLETEMATCH_CXX

#include "./CompleteMatch.h"
namespace csl {

    CompleteMatch::CompleteMatch(Alphabet& alph, char* dbFile, int dlev) : alph_(alph), db_(alph_,dbFile) {
	for(int i = 0; i < Global::Perm::maxNrOfTokens; ++i) {
	    ld_[i] = new LevDEA(alph_,dlev);
	}
	tokenDelim = alph_.code(Global::Perm::tokenDelimiter);
//	compDelim = alph_.code(Global::Perm::componentDelimiter);
	compDelim = 0;
    }

    int CompleteMatch::query(uchar* query) {
	uchar* begin = query;
	nrOfTokens_ = 0;

	while(*query) { 
	    if(*query == Global::Perm::tokenDelimiter) {
		*query = 0;
		ld(nrOfTokens_).loadPattern(begin);
		begin = query + 1;
		++nrOfTokens_;
	    }
	    ++query;
	}

	nrOfResults_ = 0;
	query_rec(db_.getRoot(), 0, LevDEA::Pos(0,0));

	return nrOfResults_;
    }

    void CompleteMatch::query_rec(int dbPos, int token, LevDEA::Pos levPos) { // levPos as reference??
	static int newDbPos;
	static LevDEA::Pos newLevPos;
	int c;

	for(c = 1; c <= alph_.size(); ++c) {
	    if((newDbPos = db_.walk(dbPos,c))) {
		if((c == tokenDelim) || (c == compDelim)) {
		    if(ld(token).isFinal(levPos)) { // if levpos is final
			if((token==nrOfTokens_-1) && db_.isFinal(newDbPos)) {
			    TransTable::AnnIterator it(db_, newDbPos);
			    while(it.isValid()) {
				results_[nrOfResults_++] = *it;
				++it;
			    }
			}
			query_rec(newDbPos, token +1, LevDEA::Pos(0,0));
		    }
		}
		else { // c is normal text symbol
		    if((newLevPos = ld(token).walk(levPos,c)).position() != -1) {
			// no match reports here: all db entries have a delimiter as last symbol
			query_rec(newDbPos, token, newLevPos);
		    }
		}
	    } //if could walk in db
	} // for all c
    }
    



} //eon

#endif

