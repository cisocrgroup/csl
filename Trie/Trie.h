#ifndef CSL_TRIE_H
#define CSL_TRIE_H CSL_TRIE_H

#include<fstream>
#include<vector>

#include "../Global.h"
#include "../TransTable/TransTable.h"
#include "../Alphabet/Alphabet.h"

namespace csl {

    class Trie : public TransTable< BASIC > {
    public:
	typedef TransTable< BASIC > TransTable_t;
	Trie( char* binFile = NULL );

	/**
	 * The funtion that actually executes the computation of the trie.
	 * @param txtFile The dictionary (including annotations) in txt format
	 * @param compFile The name of the output binary
	 */
	void compileDic(char* txtFile, char* compFile);

	/**
	 * processes one input line: separates the key from the annotations (if present)
	 * and performs the insertion into the trie
	 * @param itemString a cstring pointing to the current line
	 */
	void addToken(wchar_t* itemString);

	/// extracts the trie to stdout
	void printDic( int initState = 0 ) const;

	/// set the flag annotateByteOffset_
	inline void setByteAnnotation(bool b = true) {
	    annotateByteOffset_ = b;
	}
	
	/// set the flag annotateWordCount_
	inline void setCountAnnotation(bool b = true) {
	    annotateWordCount_ = b;
	}

	inline int walkStr(int state, const wchar_t* str) const {
	    while(*str && state) {
		state = TransTable< BASIC >::walk(state, *str );
		++str;
	    }
	    return state;
	}


    private:

	/// used to store the byte offset for the currently processed item
	int byteOffset_;
	/// flag: tells whether to annotate byte offset if no other other annotation is given
	bool annotateByteOffset_; 
	bool annotateWordCount_; 

	mutable int count_; // is used for counting during construction or printing

	// Those vars are used mostly in addToken()
	std::vector<TempState>* tempStates_;
	wchar_t* key;
	wchar_t lastKey[Global::lengthOfLongStr];
	wchar_t* valueString;

	void printDic_rec(int pos, int depth) const;

	void dotShot() const;

    };

} //eon

// #include "./Trie.cxx"

#endif
