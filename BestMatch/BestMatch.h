#ifndef BESTMATCH_H
#define BESTMATCH_H BESTMATCH_H

#include "../LevNDEA/LevNDEA.h"
#include "../Trie/Trie.h"
#include "../LevFilter/LevFilter.h"
#include "../ResultSet/ResultSet.h"

namespace csl {


/**
Class BestMatch is an implementation of an efficient algorithm to perform approximate searching in dictionaries.
The algorithm is described in Uli Reffle's master thesis.

BestMatch implements the interface LevFilter.

\author Uli Reffle, <uli@reffle.de>
\date 2006
 */
    class BestMatch : public LevFilter {
    private:
    
	enum{FW,BW};

	Alphabet& alph;

	Trie* dic[2];

	int cd; // ~ current direction
	int od; // ~ other direction
	int hd; // ~ higher-bound direction

	LevNDEA la;


	uchar pattern[2][Global::lengthOfWord]; ///< holds the current search pattern and the reverse pattern
	int patLength; ///< holds strlen(pattern)
    
	LevFilter::ResultSet_if* resultSet; ///< a container for the storage of the result
	ResultSet tmpResults;
    
	int walkCount[2];

	int walkParallel(int depth, int dicpos);

	struct StackItem {
	    int la;
	    int dicpos;
	};


	inline void changeDir() {
	    cd = od;
	    od = (od+1)%2; // toggle between directions
//	std::cout<<"Changedir to "<<cd<<std::endl;
	}

  

	/**
	 */
	void storeHit(uchar* word, int value, int dist);


	uchar word[2][Global::lengthOfWord];
	uchar revWord[Global::lengthOfWord];


	class Vars {
	public:
	    int depth;
	    int dicpos;
	    LevNDEA::SuggestIter suggest;

	    Vars(int initDicpos, LevNDEA& la, int initDepth) : suggest(la, initDepth) {
		dicpos = initDicpos;
		depth = initDepth;
	    }
	};



    public:
	BestMatch(Alphabet&, char* dicFile, char* revDicFile, int maxDist = -1);
	void loadPattern(const uchar* p);
	int query(const uchar* pat, ResultSet_if& output);

    };


} // eon

#endif
