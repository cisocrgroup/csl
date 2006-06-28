#ifndef ENTRY_H
#define ENTRY_H ENTRY_H

#include<algorithm> 
#include "fstream"
#include "../../Global.h"
#include "../../Alphabet/Alphabet.h"

namespace csl {

    class Entry {
    private:
	const Alphabet& alph_;
	uchar tokenStr_[Global::lengthOfLongStr];
	uchar* types_[Global::Perm::maxNrOfTokens];
	int length_[Global::Perm::maxNrOfTokens];
	uchar str_[Global::lengthOfLongStr];
	int v_[Global::Perm::maxNrOfTokens];

	int nrOfTokens_;

    public:

	Entry(const Alphabet& alph) : alph_(alph) {
	}
	
	
	bool loadEntry(std::ifstream& fileHandle) {
	    if(!fileHandle.getline((char*)tokenStr_, Global::lengthOfLongStr)) {
		return false;
	    }
	    if(tokenStr_[Global::lengthOfLongStr -1] != 0) {
		std::cerr<<"Length of input string is restricted to Global::lengthOfLongStr="<<Global::lengthOfLongStr<<std::endl;
		exit(1);
	    } // check for oversized input!!!

	    uchar *pos, *begin;

	    nrOfTokens_ = 0;
	    int nrOfTypes = 0;
	    pos = begin = tokenStr_;

	    while((pos = (uchar*)strchr((char*)pos,Global::Perm::tokenDelimiter))) {
		*pos = 0;

		if((nrOfTypes==0) || strcmp((char*)begin,(char*)types_[nrOfTypes-1])) {
		    types_[nrOfTypes] = begin;
		    length_[nrOfTypes] = strlen((char*)begin);
		    ++nrOfTypes;
		}

//		printf("%d -> %s\n",nrOfTokens_,(char*)tokens_[nrOfTokens_]);
		v_[nrOfTokens_] = nrOfTypes-1;
		++nrOfTokens_;
		begin = ++pos;

	    }
//	    std::sort(tokens_,tokens_+nrOfTokens_);
	    makeStr();
	    return true;
	}

	uchar* getStr() {
	    return str_;
	}

	void printV() {
	    for(int i=0;i<nrOfTokens_;++i) {
		printf("(%d,%s),",v_[i],types_[v_[i]]);
	    }
	    printf("\n");
	}

	bool nextPerm() {
	    if(std::next_permutation(v_,v_+nrOfTokens_)) {
		makeStr();
		return true;
	    }
	    else {
		makeStr(); // not really necessary
		return false;
	    }
	}


	void makeStr() {
	    uchar* pos = str_;
	    for(int i=0; i<nrOfTokens_;++i) {
		strcpy((char*)pos,(char*)types_[v_[i]]);
		pos += length_[v_[i]]; 
		*(pos++) = Global::Perm::tokenDelimiter;
	    }
	}
	
    };


} // eon


#endif
