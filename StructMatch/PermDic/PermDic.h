#ifndef PERMDIC_H
#define PERMDIC_H PERMDIC_H


#include<iostream>
#include<algorithm>

#include "./Entry.h"
#include "../../MinDic/MinDic.h"

namespace csl {
    
    class PermDic : public MinDic {
    private:
	const Alphabet& alph_; // only to pass it on to Entry

	static const int maxNrOfEntries = 100000;
	int nrOfEntries_;
	
	class ChEntry : public Entry {
	private:
	    ChEntry* next_;
	public:
	    ChEntry(const Alphabet& alph) : Entry(alph) {
		setNext(NULL);
	    }
	    ChEntry* getNext() {return next_;}
	    void setNext(ChEntry* next) {next_ = next;}
	};
	ChEntry* chain_;
	
    public:
	PermDic(const Alphabet& alph, const char* filename);
	~PermDic();

	void compileDic(char* dicFile);
    };
}


#endif
