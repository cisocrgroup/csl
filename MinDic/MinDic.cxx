#ifndef CSL_MINDIC_CXX
#define CSL_MINDIC_CXX CSL_MINDIC_CXX


#include "./MinDic.h"

namespace csl {

    MinDic::MinDic ( const Alphabet& alph, char* binFile ) : TransTable_t ( alph.size(), binFile ),
							     alph_ ( alph ) {
    }


    void MinDic::initConstruction() {
	TransTable_t::initConstruction();
	tempStates_ = ( TempState_t* ) malloc ( Global::lengthOfStr * sizeof ( TempState_t ) ); // allocate memory for all tempStates
	for ( int i = 0; i < Global::lengthOfStr; ++i ) {
	    new ( & ( tempStates_[i] ) ) TempState_t ( alph_.size() ); // call constructor for all tempStates
	}

	hashtable_ = new StateHash ( *this );
	count_ = 0;
	*lastKey = 0; // reset the string to ""
    }

    void MinDic::finishConstruction() {
	// store the very last word
	int i = strlen ( ( char* ) lastKey );
	int storedState = 0;
	for ( ; i >= 0; --i ) {
	    storedState = replaceOrRegister ( tempStates_[i] );
	    tempStates_[i].reset();
	    if ( i > 0 ) tempStates_[i-1].addTransition ( alph_.code ( lastKey[i-1] ), storedState, tempStates_[i].getPhValue() );
	}

	setRoot ( storedState ); // declare tempStates_[0] to be the root

	// call destructors for tempStates and free the allocated memory
	for ( int i = 0; i < Global::lengthOfStr; ++i ) {
	    tempStates_[i].~TempState();
	}
	free ( tempStates_ );
	delete ( hashtable_ );

	TransTable_t::finishConstruction();
    }

    void MinDic::compileDic ( char* txtFile, char* compFile ) {
	initConstruction();

	std::ifstream fileHandle ( txtFile );
	if ( !fileHandle.good() ) {
	    std::cerr << "Couldn't open dictionary file: " << txtFile << std::endl;
	    exit ( 1 );
	}

	uchar line[Global::lengthOfLongStr];

	while ( fileHandle.getline ( ( char* ) line, Global::lengthOfLongStr ) )  {
	    //    std::cerr<<count_<<"\t"<<line<<std::endl; //DEBUG!

	    addToken ( line );

	    if ( ! ( count_ %  10000 ) ) {
		std::cerr << "\r" << count_ << " tokens processed." << std::flush;
	    }
	}

	fileHandle.close();

	finishConstruction();

	// dump memory to file
	createBinary ( compFile );
    }


    void MinDic::addToken ( const uchar* key ) {
	static int commonPrefix, i, lengthOfKey;
	static int storedState;

	lengthOfKey = strlen ( ( char* ) key );
	if ( lengthOfKey > Global::lengthOfStr ) {
	    std::cerr << "Error: Automaton: Global::string_length sets the maximum string length for an entry of a trie: " << Global::lengthOfStr << std::endl;
	}

	// check alphabetical order
	if ( *key && alph_.strcmp ( lastKey, key ) > 0 ) {
	    std::cout << "Alphabetical order violated:" << std::endl
		      << lastKey << std::endl
		      << key << std::endl;
	    exit ( 1 );
	}

	///////////////////// store suffix of lastKey
	commonPrefix = 0;
	while ( key[commonPrefix] == lastKey[commonPrefix] && key[commonPrefix] ) {
	    ++commonPrefix;
	}
	// e.g., commonPrefix==2 if first two letters of both words are equal

	i = strlen ( ( char* ) lastKey );
	for ( ; i > commonPrefix; --i ) {
	    storedState = replaceOrRegister ( tempStates_[i] );
	    tempStates_[i-1].addTransition ( alph_.code ( lastKey[i-1] ), storedState, tempStates_[i].getPhValue() );
	    tempStates_[i].reset();
	}


	//////////////////// set final state of key
	tempStates_[lengthOfKey].setFinal ( true );

	strcpy ( ( char* ) lastKey, ( char* ) key );
	++count_;
    }


    void MinDic::printDic() const {
	count_ = 0;
	printDic_rec ( getRoot(), 0 );
    }

    void MinDic::printDic_rec ( int pos, int depth ) const {
	int newPos;
	static uchar w[Global::lengthOfStr];

	TransIterator trans = getTransIterator( pos );
	while ( trans.isValid() ) {
	    if ( ( newPos = walk ( pos, *trans ) ) ) {
		w[depth] = alph_.decode ( *trans );

		if ( isFinal ( newPos ) ) {
		    w[depth+1] = 0;
		    printf ( "%s\n", ( char* ) w );

		    if ( ( ++count_ % 100000 ) == 0 ) fprintf ( stderr, "%d\n", count_ );
		} // if isFinal

		printDic_rec ( newPos, depth + 1 );

	    } // if couldWalk
	    ++trans;
	} // while
    } // end of method



} //eon


#endif
