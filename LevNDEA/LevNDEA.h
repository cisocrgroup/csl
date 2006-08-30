#ifndef LEVNDEA_H
#define LEVNDEA_H LEVNDEA_H

#include<iostream>
#include "../Global.h"
#include "../Alphabet/Alphabet.h"

namespace csl {

    /**
     * LevNDEA is to represent a nondeterministic automaton computing levenshtein distances.
     *
     * The design of the automaton is the well-known one described e.g. in ???
     *
     * +---+---+---+---+---+---+---+
     * | / | / | / | / | / | / | / |
     * +---+---+---+---+---+---+---+
     * | / | / | / | / | / | / | / |
     * +---+---+---+---+---+---+---+
     *   p   a   t   t   e   r   n
     *
     * @author Ulrich Reffle
     * @date 2006
     */
    class LevNDEA {
    private:
	Alphabet& alph;


	// we need stacksize = word_length+1: one for reading each character, one for the start configuration
	static const int stacksize = Global::lengthOfWord+1;

	bits64 floors[stacksize][Global::lengthOfWord];

	uchar pattern[Global::lengthOfWord];
	int patLength;

	uchar allChars[Global::lengthOfWord]; ///< holds all characters appearing in the pattern
  
	bits64* charvecs;

	int maxFloorIndex;

	static const bits64 firstBit = 1ll << 63; // that's a '1' followed by 63 '0's
	bits64 lastBit; // the lowest needed bit
	bits64 allBits; // all needed bits



	////////PRIVATE METHODS


	void calcCharvec();
    

    public:
	LevNDEA(Alphabet& alph);
	~LevNDEA();

#include "SuggestIter.h"  

	void loadPattern(const uchar* p, int maxDist = -1);
	bool walk(uchar c, int stackIndex);

	inline int isFinal(int stackIndex) const {
	    int curFloor = 0;
	    while((curFloor <= maxFloorIndex) && !(floors[stackIndex][curFloor] & lastBit)) {
		++curFloor;
	    }
	    return (curFloor <= maxFloorIndex)? curFloor : -1;
	}


	inline void setMaxDist(int d) {
	    maxFloorIndex = d;
	}

	inline int getMaxDist() const {
	    return maxFloorIndex;
	}

	inline const bits64& getFloor(int stackIndex, int floor) const {
	    return floors[stackIndex][floor];
	}

	inline const uchar* getAllChars() const {
	    return allChars;
	}
    
	inline const bits64& charvec(int c) const {
	    return charvecs[c];
	}


	void printFloor(int stackIndex, int floor);
	void printFloors(int stackIndex);
	void printBits(bits64);
    };

} //eon
#endif
