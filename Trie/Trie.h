#ifndef CSL_TRIE_H
#define CSL_TRIE_H CSL_TRIE_H

#include<fstream>
#include<stdexcept>

#include "../Global.h"
#include "../TransTable/TransTable.h"
#include "../Alphabet/Alphabet.h"

namespace csl {

    class Trie : public TransTable< BASIC > {
    public:
	typedef TransTable< BASIC > TransTable_t;
	inline Trie(const Alphabet& alph, char* binFile = NULL);

	/**
	   The funtion that actually executes the computation of the trie.
	   @param txtFile The dictionary (including annotations) in txt format
	   @param compFile The name of the output binary
	*/
	inline void compileDic(char* txtFile, char* compFile);

	/**
	   processes one input line: separates the key from the annotations (if present)
	   and performs the insertion into the trie
	   \arg a cstring pointing to the current line
	*/
	inline void addToken(uchar* itemString);

	/// extracts the trie to stdout
//	inline void printDic(StateId initState,uchar* prefix = (uchar*)"") const;
	inline void printDic(int initState) const;

	/// set the flag annotateByteOffset_
	inline void setByteAnnotation(bool b = true) {
	    annotateByteOffset_ = b;
	}
	
	/// set the flag annotateWordCount_
	inline void setCountAnnotation(bool b = true) {
	    annotateWordCount_ = b;
	}

	inline int walkStr(int state, const uchar* str) const {
	    while(*str && state) {
		state = TransTable< BASIC >::walk(state, alph_.code(*str));
		++str;
	    }
	    return state;
	}

    private:
	const Alphabet& alph_;

	/// used to store the byte offset for the currently processed item
	int byteOffset_;
	/// flag: tells whether to annotate byte offset if no other other annotation is given
	bool annotateByteOffset_; 
	bool annotateWordCount_; 

	mutable int count_; // is used for counting during construction or printing

	// Those vars are uses mostly in addToken()
	TempState_t *tempStates_;
	uchar* key;
	uchar lastKey[Global::lengthOfLongStr];
	uchar *valueString;

	inline void printDic_rec(int pos, int depth) const;

    };

} //eon

#include "./Trie.cxx"

#endif
