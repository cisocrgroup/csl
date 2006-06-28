#ifndef LEVNDEA_CXX
#define LEVNDEA_CXX LEVNDEA_CXX

#include "LevNDEA.h"

namespace csl {

    LevNDEA::LevNDEA(Alphabet& initAlph) : alph(initAlph) {
	charvecs = new bits64[alph.size()+1];
	pattern[0] = 0;
    }

    LevNDEA::~LevNDEA() {
	delete[] charvecs;
    }


    void LevNDEA::loadPattern(const uchar* p, int maxDist) {
	int i; // for some loops
    
	strcpy((char*)pattern, (char*)p);
	memset(floors,0,(Global::lengthOfWord * Global::lengthOfWord)); // prepare memory
	patLength = strlen((char*)pattern);
    
	calcCharvec();

    
	maxFloorIndex = (maxDist!=-1)? maxDist : patLength;
  
	lastBit = firstBit>>patLength; 
	allBits = 0;
	for(i=0;i<=patLength;++i) { // is there a better way to do that?
	    allBits |= firstBit>>i;
	}
  
	floors[0][0] = firstBit; // activate start state of lower stackframe
	// epsilon closure for lowest automaton of stack: activate diagonal line 
	for(i=0; i<=maxFloorIndex; ++i) {
	    floors[0][i] = firstBit>>i;
	}
  
//  printFloors(0);

	//    std::cout<<"New pattern: '"<<pattern<<"' with length "<<patLength<<std::endl;
    }

    void LevNDEA::calcCharvec() {
	bits64 c = firstBit;
	int i;
   
	memset(charvecs, 0, (alph.size()+1)*sizeof(bits64));
	int nrOfChars = 0;
	for (i=0; pattern[i]; i++, c>>=1) {
	    if(charvecs[alph.code(pattern[i])] == 0) { //if char appeared for the 1st time
		allChars[nrOfChars++] = alph.code(pattern[i]);
		allChars[nrOfChars] = 0;
	    }
	    charvecs[alph.code(pattern[i])] |= c;
	}
    }

    bool LevNDEA::walk(uchar c, int stackIndex) {
	static int curFloor;
	static bool couldWalk;

	// horizontal transition for lower floor
	floors[stackIndex+1][0] = (floors[stackIndex][0] & charvec(c))>>1;

	couldWalk = floors[stackIndex+1][0];
    

	// write updated automaton into stackIndex+1
	// check if there are active states at all (-> couldWalk)
	// check if one (and which) of the final states is active
	for(curFloor=1; curFloor <= maxFloorIndex; ++curFloor) {

	    floors[stackIndex+1][curFloor] = 
		((floors[stackIndex][curFloor] & charvec(c))>>1) | // horizontal transition
		floors[stackIndex][curFloor-1] |  // vertical transition: no matter what c is
		(floors[stackIndex][curFloor-1]>>1) |  // diagonal transition: no matter what c is (note the |= and the >>1 operator!)
		(floors[stackIndex+1][curFloor-1]>>1);  // diagonal epsilon transition: note that a shifted version of the SAME stack's lower floor is used


	    // delete bits beyond the relevant range
	    // might be unnecessary
	    floors[stackIndex+1][curFloor] &= allBits;


	    // update couldWalk
	    couldWalk = couldWalk || floors[stackIndex+1][curFloor];
      
//	printFloors(stackIndex+1);

	}
	return couldWalk;
    } 





    void LevNDEA::printFloor(int stackIndex, int floor) {
	int i;
	for(i=63;i>=63-patLength;--i) {
	    printf("%u ", (unsigned int)(1 & (floors[stackIndex][floor]>>i)));
	}
	printf("\n");
    
    }
    void LevNDEA::printFloors(int stackIndex) {
	int i;
	for(i=maxFloorIndex;i>=0;--i) {
	    printf("%2d ** ",i);
	    printFloor(stackIndex,i);
	}
	printf("       ");

	for(i=0;pattern[i];++i) {
	    printf("%c ",pattern[i]);
	}
	printf("\n");

    }

    void LevNDEA::printBits(bits64 n) {
	int i;
	for(i=63;i>=0;--i) {
	    if((i%10)==0) printf("%d",i/10);
	    else if((i%10)==5) printf("|");
	    else printf(" ");
	}
	printf("\n");
	for(i=63;i>=0;--i) {
	    printf("%u", (unsigned int)(1 & (n>>i)));
	}
	printf("\n");
    
    }



} // eon




#endif
