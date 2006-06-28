#ifndef PERMDIC_CXX
#define PERMDIC_CXX PERMDIC_CXX

#include "./PermDic.h"
#include "../../MinDic/MinDic.h"

namespace csl {
    PermDic::PermDic(const Alphabet& alph, const char* txtFile) : MinDic(alph), alph_(alph) {
	
	nrOfEntries_ = 0;
	// load entries into Entry-objects
	std::ifstream fileHandle(txtFile);
	if(!fileHandle.good()) {
	    std::cerr<<"Couldn't open dictionary file: "<<txtFile<<std::endl;
	    exit(1);
	}
	nrOfEntries_ = 0;

	// read first entry; 
	chain_ = new ChEntry(alph_);
	chain_->loadEntry(fileHandle);

	ChEntry* e;
	ChEntry* last = chain_;
	while((e = new ChEntry(alph_)) && (e->loadEntry(fileHandle)))  {
	    last->setNext(e);
	    last = e;
	    ++nrOfEntries_;
	}
    }

    PermDic::~PermDic() {
    }


    /**
       Compiles the automaton and dumps it to dicFile
    */
    void PermDic::compileDic(char* dicFile) {
	MinDic::initConstruction();
	int count = 0;
	while(chain_) {
//	    chain_->printV();
//	    printf("%s\n",(char*)chain_->getStr());
	    MinDic::addToken(chain_->getStr());
	    if(++count % 10000 == 0) fprintf(stderr,"%dk lines processed\n",count/1000);
	    
	    if(chain_->nextPerm()) { // there are more perms for this entry
		ChEntry* e = chain_;
		while(e->getNext() && (strcmp((char*)chain_->getStr(), (char*)e->getNext()->getStr()) > 0)) {
		    e = e->getNext();
		}
		if(e != chain_) {
		    ChEntry* newChain = chain_->getNext();
		    
		    chain_->setNext(e->getNext());
		    e->setNext(chain_);

		    chain_ = newChain;
		}
	    }
	    else {
//		printf(">>%d - %s\n",chain_->debugCount_,(char*)chain_->getStr());

		ChEntry* tmp = chain_->getNext();
		delete chain_;
		chain_ = tmp;
	    }

	}
	MinDic::finishConstruction();

	TransTable::createBinary(dicFile);
    }


} // eon

#endif
