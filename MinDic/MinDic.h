#ifndef CSL_MINDIC
#define CSL_MINDIC CSL_MINDIC

#include "fstream"
#include "../TransTable/TransTable.h"
#include "../Alphabet/Alphabet.h"

namespace csl {


/**
   class MinDic performs the construction and usage of minimal acyclic 
   finite state automata for large finite dictionaries. 
   It follows an algorithm described in 
   Jan Daciuk et. al., Incremental Construction of Minimal Acyclic
   Finite-State Automata, 2000.
 
   Most ideas for the implementation are 'stolen' from a C-program written
   by Stoyan Mihov.
 
   \author Uli Reffle, <uli@reffle.de>
   \date Apr 2006
 
 
*/
    class MinDic : public TransTable< TOKDIC > {
    public:
	typedef TransTable< TOKDIC > TransTable_t;

	MinDic ( const Alphabet& alph, char* binFile = NULL );

	/**
	   The funtion that actually executes the computation of the trie.
	   @param txtFile The dictionary (including annotations) in txt format
	   @param compFile The name of the output binary
	*/
	void compileDic ( char* txtFile, char* compFile );


	void initConstruction();
	void finishConstruction();

	/**
	   processes one input line: separates the key from the annotations (if present)
	   and performs the insertion into the trie
	   \arg a cstring pointing to the current line
	*/
	void addToken ( const uchar* key );

	/// extracts the trie to stdout
	void printDic ( int initState ) const;


	int walkStr ( int state, const uchar* str ) const {
	    while ( *str && state ) {
		state = walk ( state, alph_.code ( *str ) );
		++str;
	    }
	    return state;
	}

	void printDic() const;

    private:
#include "./StateHash.h"
	const Alphabet& alph_;

	mutable int count_; // is used for counting during construction or printing

	// Those vars are used for construction
	TempState_t *tempStates_;
	StateHash* hashtable_;

	uchar* key;
	uchar lastKey[Global::lengthOfLongStr];
	uchar *valueString;

	inline int replaceOrRegister ( TempState_t& state ) {
	    int storedState = 0;
	    if ( ( storedState = hashtable_->findState ( state ) ) == 0 ) { // if equiv. state doesn't exist
		storedState = storeTempState ( state ); // store it
		hashtable_->push ( state, storedState ); // add it to the hashtable
        }
        return storedState;
    }


    void printDic_rec ( int pos, int depth ) const;

};

} //eon

#include "./MinDic.cxx"

#endif
