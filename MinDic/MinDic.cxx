#ifndef CSL_MINDIC_CXX
#define CSL_MINDIC_CXX CSL_MINDIC_CXX


#include "./MinDic.h"

namespace csl {

    MinDic::MinDic( const Alphabet& alph ) : TransTable_t( alph.size() ),
					     alph_( alph ) {
    }
    

    void MinDic::initConstruction() {
	TransTable_t::initConstruction();
	tempStates_ =( TempState_t* ) malloc( Global::lengthOfStr * sizeof( TempState_t ) ); // allocate memory for all tempStates
	for( int i = 0; i < Global::lengthOfStr; ++i ) {
	    new( &( tempStates_[i] ) ) TempState_t( alph_ ); // call constructor for all tempStates
	}

	hashtable_ = new StateHash( *this );

	sizeOfAnnotationBuffer_ = 100000;
	annotations_ = (int*)malloc( sizeOfAnnotationBuffer_ * sizeof( int ) );
	
	
	header_.nrOfKeys_ = 0;
	*lastKey_ = 0; // reset the string to ""
    }

    void MinDic::finishConstruction() {
	// store the very last word
	int i = strlen( ( char* ) lastKey_ );
	int storedState = 0;
	for( ; i >= 0; --i ) {
	    storedState = replaceOrRegister( tempStates_[i] );
	    tempStates_[i].reset();
	    if( i > 0 ) tempStates_[i-1].addTransition( alph_.code( lastKey_[i-1] ), storedState, tempStates_[i].getPhValue() );
	}

	setRoot( storedState ); // declare tempStates_[0] to be the root

	// call destructors for tempStates and free the allocated memory
	for( int i = 0; i < Global::lengthOfStr; ++i ) {
	    tempStates_[i].~TempState();
	}
	free( tempStates_ );
	delete( hashtable_ );

	TransTable_t::finishConstruction();
    }

    void MinDic::compileDic( char* txtFile ) {
	uchar* key = 0;
	uchar* valueString = 0;

	initConstruction();

	std::ifstream fileHandle( txtFile );
	if( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" + 
					     std::string( txtFile ) + 
					     "' for reading." );
	}
	uchar line[Global::lengthOfLongStr];
	line[Global::lengthOfLongStr - 1] = 0;

	while( fileHandle.getline(( char* ) line, Global::lengthOfLongStr ) )  {
// 	    if( line[Global::lengthOfLongStr] != 0 ) {
// 		throw exceptions::badInput( "MinDic: length of input line exceeds maximium" );
// 	    }
	    
	    /////////////////// PARSE THE INPUT STRING
	    uchar *c;
	    c = ( uchar* )strchr( ( char* )line, Global::keyValueDelimiter );
	    
	    if( c ) {
		*c = 0;
		valueString = ( c + 1 );
	    } 
//	    strcpy( (char*)key_, (char*)input );
	    key = line;

	    addToken( key, ( ( c )? atoi( (char*)valueString ) : 0 ) );

	}

	fileHandle.close();

	finishConstruction();

    }


    void MinDic::addToken( const uchar* key, int value ) {
	static int commonPrefix, i, lengthOfKey;
	static int storedState;

	key_ = key;

//	printf("input: %s\n", input );


	lengthOfKey = strlen( ( char* )key_ );
	if( lengthOfKey > Global::lengthOfStr ) {
	    throw exceptions::bufferOverflow( "Trie.cxx: Global::string_length sets the maximum string length for an entry of a trie. Maximum violated" );
	}
	
	// check alphabetical order
	if( *key_ && alph_.strcmp( lastKey_, key_ ) > 0 ) {
	    std::cout << "Alphabetical order violated:" << std::endl
		      << lastKey_ << std::endl
		      << key_ << std::endl;
	    exit( 1 );
	}

	///////////////////// store suffix of lastKey_
	commonPrefix = 0;
	while( key_[commonPrefix] == lastKey_[commonPrefix] && key_[commonPrefix] ) {
	    ++commonPrefix;
	}
	// e.g., commonPrefix==2 if first two letters of both words are equal

	i = strlen(( char* ) lastKey_ );
	for( ; i > commonPrefix; --i ) {
	    storedState = replaceOrRegister( tempStates_[i] );
	    tempStates_[i-1].addTransition( alph_.code( lastKey_[i-1] ), storedState, tempStates_[i].getPhValue() );
	    tempStates_[i].reset();
	}

	//////////////////// set final state of key_
	tempStates_[lengthOfKey].setFinal( true );
	
	// store value
	if( 1 ) {
	    // see that annotation buffer is large enough
	    if( sizeOfAnnotationBuffer_ <= ( header_.nrOfKeys_ + 1 ) ) {
 		sizeOfAnnotationBuffer_ *= 2;
		annotations_ = (int*)realloc( annotations_, sizeOfAnnotationBuffer_ * sizeof( int ) );
		if( !annotations_ ) throw( exceptions::cslException( "csl::MinDic: could not re-allocate memory for annotation array." ) );
	    }
	    annotations_[header_.nrOfKeys_] = value;
	}

	strcpy(( char* ) lastKey_,( char* ) key_ );
	++(header_.nrOfKeys_);

	if( !( header_.nrOfKeys_ %  10000 ) ) {
	    std::cerr << "\r" << header_.nrOfKeys_ << " tokens processed." << std::flush;
	}
    }
    
    void MinDic::printDic() const {
	count_ = 0;
	printDic_rec( getRoot(), 0, 0 );
    }

    void MinDic::printDic_rec( int pos, int depth, size_t perfHashValue ) const {
	int newPos;
	static uchar w[Global::lengthOfStr];
	size_t newPerfHashValue;
	
	const uchar* transitions = getSusoString( pos );
	while( *transitions ) {
	    newPerfHashValue = perfHashValue;;
	    if( ( newPos = walkPerfHash( pos, *transitions, newPerfHashValue ) ) ) {
		w[depth] = alph_.decode( *transitions );

		if( isFinal( newPos ) ) {
		    w[depth+1] = 0;
		    printf( "%s#%d\n",( char* ) w, getAnnotation( newPerfHashValue ) );

		    if( ( ++count_ % 100000 ) == 0 ) fprintf( stderr, "%d\n", count_ );
		} // if isFinal
		printDic_rec( newPos, depth + 1, newPerfHashValue );

	    } // if couldWalk
	    else {
		throw exceptions::badDictFile( "suso-string seems to be corrupted." ); 
	    }
	    ++transitions;
	} // while
    } // end of method

} //eon


#endif
