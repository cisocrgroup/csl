#ifndef CSL_TRIE_CXX
#define CSL_TRIE_CXX CSL_TRIE_CXX


#include "./Trie.h"

namespace csl {

    Trie::Trie( const Alphabet& alph, char* binFile ) : TransTable< BASIC >( alph.size(), binFile ), alph_( alph ) {
	annotateByteOffset_ = false;
    }

    void Trie::compileDic( char* txtFile, char* compFile ) {
	TransTable_t::initConstruction();
	tempStates_ = ( TempState_t* ) malloc( Global::lengthOfStr * sizeof( TempState_t ) ); // allocate memory for all tempStates
	for( int i = 0; i < Global::lengthOfStr; ++i ) {
	    new( &( tempStates_[i] ) ) TempState_t( alph_.size() ); // call constructor for all tempStates
	}

	std::ifstream fileHandle( txtFile );
	if( !fileHandle.good() ) {
	    std::cerr << "Couldn't open dictionary file: " << txtFile << std::endl;
	    exit( 1 );
	}

	uchar line[Global::lengthOfLongStr];


	count_ = 0;

	byteOffset_ = 0;
	*lastKey = 0; // reset the string to ""

	while( fileHandle.getline( ( char* )line, Global::lengthOfLongStr ) )  {
	    //    std::cerr<<count_<<"\t"<<line<<std::endl; //DEBUG!

	    addToken( line );
	    ++count_;
	    if( !( count_ %  10000 ) ) {
		std::cerr << "\r" << count_ << " tokens processed." << std::flush;
	    }
	    byteOffset_ = int( fileHandle.tellg() );
	}

	fileHandle.close();

	// store the very last word
	int i = strlen( ( char* )lastKey );
	for( ; i > 0; --i ) {
	    int storedState = storeTempState( tempStates_[i] );
	    tempStates_[i].reset();
	    tempStates_[i-1].addTransition( alph_.code( lastKey[i-1] ), storedState );
	    std::cerr<<"st="<<storedState<<std::endl;
	}

	setRoot( storeTempState( tempStates_[0] ) ); // store root and write root's id into array[0]

	TransTable_t::finishConstruction();

	// dump memory to file
	createBinary( compFile );

	// call destructors for tempStates and free the allocated memory
//  for(int i=0; i< Global::lengthOfStr; ++i) {
//      tempStates_[i].~TempState();
//  }
//  free( tempStates_ );

    }


    /**
       @except out_of_range 
    */
    void Trie::addToken( uchar* input ) {

	static int commonPrefix, i, lengthOfKey;
	static int storedState;


	/////////////////// PARSE THE INPUT STRING
	uchar *c, *begin;
	c = ( uchar* )strchr( ( char* )input, Global::keyValueDelimiter );

	if( c ) {
	    *c = 0;
	    key = input;
	    valueString = ( c + 1 );
	} else { // no values given
	    key = input;
	    valueString = ( uchar* )"";
	}
	lengthOfKey = strlen( ( char* )key );
	if( lengthOfKey > Global::lengthOfStr ) {
	    throw std::out_of_range( "Trie.cxx: Global::string_length sets the maximum string length for an entry of a trie. maximum violated" );
	}

	// check alphabetical order
	if( *key && alph_.strcmp( lastKey, key ) > 0 ) {
	    std::cout << "Alphabetical order violated:" << std::endl
		      << lastKey << std::endl
		      << key << std::endl;
	    exit( 1 );
	}



	///////////////////// store suffix of lastKey
	commonPrefix = 0;
	while( key[commonPrefix] == lastKey[commonPrefix] && key[commonPrefix] ) {
	    ++commonPrefix;
	}
	// e.g., commonPrefix==2 if first two letters of both words are equal

	i = strlen( ( char* )lastKey );
	for( ; i > commonPrefix; --i ) {
	    storedState = storeTempState( tempStates_[i] );
	    tempStates_[i].reset();
	    tempStates_[i-1].addTransition( alph_.code( lastKey[i-1] ), storedState );
	}


	//////////////////// set final state of key and add all the values
	tempStates_[lengthOfKey].setFinal( true );

	if( 0 && *valueString ) { // if valueString not empty
	    begin = valueString;
	    c = begin;

	    do { // traverse valueString
		++c;
		if( *c == Global::valuesDelimiter || ( *c == 0 ) ) { // some avoidable loops are happening here,but nevermind
		    *c = 0;
		    tempStates_[lengthOfKey].addAnnotation( atoi( ( char* )begin ) ); // add annotation to tempState
		    begin = c + 1;
		}
	    } while ( *c );
	} else if( annotateByteOffset_ == true ) { // obviously there were no values given in the string.
	    tempStates_[lengthOfKey].addAnnotation( byteOffset_ ); // use byte-offset as value
	} else if( annotateWordCount_ == true ) {
	    tempStates_[lengthOfKey].addAnnotation( count_ ); // use wordcount as value
	}

	strcpy( ( char* )lastKey, ( char* )key );
    }

    void Trie::printDic( int initState ) const {
	if( initState == 0 ) initState = getRoot();
	count_ = 0;
	printDic_rec( initState, 0 );
    }

    void Trie::printDic_rec( int pos, int depth ) const {
	uint_t c;
	static int newPos;
	static uchar w[Global::lengthOfStr];

	for( c = 1; c <= alph_.size(); ++c ) {
//     std::cout<<"depth="<<depth<<", char="<<(uchar)alph_.decode(c)<<std::endl;
	    if( ( newPos = walk( pos, c ) ) ) {
		w[depth] = alph_.decode( c );

		if( isFinal( newPos ) ) {
		    w[depth+1] = 0;
		    printf( "%s", ( char* )w );

		    if( hasAnnotations( newPos ) ) {
			std::cout << Global::keyValueDelimiter;
            AnnIterator it( *this, newPos );
            while( it.isValid() ) {
              printf( "%d, ", *it );
              ++it;
            }
          }
          printf( "\n" );

          if( ( ++count_ % 100000 ) == 0 ) fprintf( stderr, "%d\n", count_ );
        } // if isFinal

        printDic_rec( newPos, depth + 1 );
      } // if couldWalk
    } // for
  } // end of method



} //eon


#endif
