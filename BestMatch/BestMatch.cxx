#ifndef BESTMATCH_CXX
#define BESTMATCH_CXX

#include "./BestMatch.h"

namespace csl {

    // the third argument is for compatibility with the interface of Levquery.
    // In Levquery, this arg gives the max levenshtein distance. This could be implemented here as well?!
    BestMatch::BestMatch(Alphabet & initAlph, char* dicFile, char* revDicFile, int) : alph(initAlph), la(alph) {
	dic[FW] = new Trie(alph, dicFile);
	dic[BW] = new Trie(alph, revDicFile);


	walkCount[FW] = 0;
	walkCount[BW] = 0;
    }

    void BestMatch::loadPattern(const uchar* p) {
	strcpy((char*)(pattern[FW]), (char*) p);
	patLength = strlen((char*)pattern[FW]);

	// set up reverse pattern
	int i;
	pattern[BW][patLength] = 0;
	for(i = 0; i<patLength; ++i) {
	    pattern[BW][patLength - 1 - i] = pattern[FW][i];
	}

	// evaluate which dir to take
	int reached[2];
	for(cd=0;cd<=1;++cd) {
	    int dicpos = dic[cd]->getRoot();
	    uchar w[Global::lengthOfWord] = "";
	    int ws = 0;
	    uchar* c = pattern[cd];
	    while(*c && (dicpos= dic[cd]->walk(dicpos, alph.code(*c)))) {
		w[ws++] = *c; w[ws]=0;
		++c;
	    }
	    reached[cd] = ws;
	}
	cd = (reached[FW] >= reached[BW])? FW : BW;
	std::cout<<"chosen dir: "<<cd<<std::endl;
    
//    cd = FW;

//    la.loadPattern(pattern[cd], patLength-1); // allow a max error rate of 1
	la.loadPattern(pattern[cd], (int)patLength/2); // allow a max error rate of 0.5
//    la.loadPattern(pattern[cd], 1); // allow a max k of 1

    }


/**
   @returns the lev distance of the best match found
*/
    int BestMatch::query(const uchar* pat, LevFilter::ResultSet_if& output) {
	resultSet = &output;
	tmpResults.reset();
	loadPattern(pat);
    
	int dist = walkParallel(0,dic[cd]->getRoot());
	for(ResultSet::Iterator it= tmpResults.begin();it != tmpResults.end();++it) {
	    resultSet->push((*it).getStr(), (*it).getAnn());
	}
	return dist;
    }


    int BestMatch::walkParallel(int depth, int dicpos) {
	static uchar word[Global::lengthOfWord];

	int newDicpos, dist;
	int c;

	word[depth] = 0; // ?
	
//	std::cout<<"word: "<<word<<std::endl;
//      la.printFloors(depth); //DEBUG!
//      std::cout<<std::endl;


	LevNDEA::SuggestIter suggest(la,depth);
    
	while((c = *suggest)) {
//	std::cout<<"Suggest:"<<(char)alph.decode(c)<<std::endl;
	    if((newDicpos = dic[cd]->walk(dicpos,c)) && (la.walk(c,depth))) {
		
		word[depth] = alph.decode(c);
//	    std::cout<<"word:"<<word<<std::endl;
		if(dic[cd]->isFinal(newDicpos) && (dist = la.isFinal(depth+1))!=-1) { //TREFFER!!!
		    word[depth+1] = 0;
		    storeHit(word, dic[cd]->getFirstAnn(newDicpos), dist);
		}
		walkParallel(depth+1, newDicpos);
	    }
	    ++suggest;
	}

    
//    if(depth==0) std::cout<<pattern<<" --> "<<resultList->token(0)<<", Abstand: "<<maxFloorIndex<<" ("<<resultList->size()<<" Treffer insgesamt)"<<std::endl;


	return la.getMaxDist();
    }




    void BestMatch::storeHit(uchar* word, int value, int dist) {
	static uchar reversedWord[Global::lengthOfWord];
	bool success;
	if(dist < la.getMaxDist()) {
	    tmpResults.reset();
	    la.setMaxDist(dist); // forget floors above the current hit
	}
	if(cd == BW) {
	    Global::reverse(word, reversedWord);
	    success = tmpResults.push(reversedWord,value);
	}
	else {
	    success = tmpResults.push(word,value);
	}
	if(!success) {
	    std::cerr<<"BestMatch: resultList overflow. Exiting."<<std::endl;
	    exit(1);
	}
    }

} // eon

#endif
