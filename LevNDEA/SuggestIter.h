#ifndef SUGGESTITER_H
#define SUGGESTITER_H SUGGESTITER_H

class SuggestIter { // of class LevNDEA
 private: 
    LevNDEA& la;
    int stackIndex;

    // the currently suggested character
    int c;

    // floor and charPos to remember the position during intelligent suggesting
    int floor;
    const uchar* charPos;

    bool intelMode;

    /** 
	when a character c is checked, its charvec is or'd with this bitvector.
	That way we can remember which chars have already been checked on lower floors
    */
    bits64 checked;

 public:
    SuggestIter(LevNDEA& initLa, int si = 0) : la(initLa), stackIndex(si) {
	reset();
	intelMode = true;
    }
	
    int operator*() const {
	return c; 
    }

    // find an intelligent suggestion in the automaton
    // may only be called with floor <= la.getmaxDist()
    int findSuggestion() {
	if(floor > la.getMaxDist()) {
	    std::cerr<<"SuggestIter::findSuggestion: STRANGE BEHAVIOUR, this should not happen!"<<std::endl;
	    exit(1);
	}

	while(((la.charvec(*charPos) & checked) != 0) // while *charPos was checked before
	      || (la.charvec(*charPos) & la.getFloor(stackIndex,floor)) == 0) { //OR *charPos is no good idea
	    ++charPos;
	    if(!(*charPos)) { // tried all chars for current floor
		++floor; // try next floor
		charPos = la.getAllChars(); // reset charPos to beginning of allChars
		if(floor > la.getMaxDist()) { // all floors processed, no more intelligent guesses
		    intelMode = false;
		    c = 0; // initialise c for going through whole alphabet
		    return 0; 
		}
	    }
	}
	checked |= la.charvec(*charPos);
	return *charPos;
    }
    
    SuggestIter& operator++() {
	if(intelMode && (floor > la.getMaxDist())) {
	    c=0;
	    intelMode=false;
	}
	if((intelMode)
	   && (++charPos)
	   && (c = findSuggestion())
	    ) {
	    // could find another intelligent guess
	}
	else {
	    while((++c) <= la.alph.size() && (la.charvec(c) & checked) != 0 ); // assign to c the next char of the alph that wasn't checked before
	}
	if(c>la.alph.size()) {
	    c = 0;
	}
	return *this;
    }

    void reset() {
	floor = 0;
	charPos = la.getAllChars();
	c = 1;
	checked = 0ll;

	c = findSuggestion();
    }

    bool intelligentMode() {
	return (floor > la.getMaxDist());
    }
      
}; // End class SuggestIter

#endif
