#ifndef CSL_MINDICSTRING_TCC
#define CSL_MINDICSTRING_TCC CSL_MINDICSTRING_TCC

namespace csl {

    inline MinDicString::MinDicString() :
	annStrings_( 0 ),
	sizeOfAnnStrings_( 0 ),
	annHash_( 0 ),
	keyValueDelimiter_( Global::keyValueDelimiter )
    {
	
    }

    inline void MinDicString::setKeyValueDelimiter( uchar c ) {
	keyValueDelimiter_ = c;
    }

    inline const uchar* MinDicString::lookup( wchar_t* key ) const {
	int annInt = 0;
	if( MinDic_t::lookup( key, &annInt ) )
	    return annStrings_ + annInt;
	return 0;
    }

    inline const uchar* MinDicString::getAnnByPerfHashValue( size_t perfHashValue ) const {
	size_t offset = annotationsAt( perfHashValue );
	assert( offset < sizeOfAnnStrings_ );
	return annStrings_ + offset;
    }


    inline void MinDicString::loadFromFile( char* dicFile ) {
	FILE* fi = fopen( dicFile, "rb" );
	if ( !fi ) {
	    throw exceptions::badFileHandle( "csl::MinDicString: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for reading." );
	}
	loadFromStream( fi );
	fclose( fi );
    }
    
    inline void MinDicString::loadFromStream( FILE* fi ) {
	fread( &header_, sizeof( Header ), 1, fi );
	if ( ( header_.getMagicNumber() != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "MinDicString: Magic number comparison failed.\n" );
	}
	
	sizeOfAnnStrings_ = header_.getSizeOfAnnStrings();
	MinDic_t::loadFromStream( fi );
	annStrings_ = (uchar*) malloc( sizeOfAnnStrings_ * sizeof( uchar ) );
	fread( annStrings_, sizeof( uchar ), sizeOfAnnStrings_, fi );
	
    }

    inline void MinDicString::writeToFile( char* dicFile ) const {
	FILE* fo = fopen( dicFile, "wb" );
	if ( !fo ) {
	    throw exceptions::badFileHandle( "MinDicString: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for writing." );
	}
	writeToStream( fo );
	fclose( fo );
    }
    
    inline void MinDicString::writeToStream( FILE* fo ) const {
	fwrite( &header_, sizeof( Header ), 1, fo );
	MinDic_t::writeToStream( fo );
	fwrite( annStrings_, sizeof( uchar ), sizeOfAnnStrings_, fo );
    }


    inline void MinDicString::initConstruction() {
	MinDic_t::initConstruction();
	
    }

    inline void MinDicString::finishConstruction() {
	MinDic_t::finishConstruction();
	header_.set( *this );
    }

    inline void MinDicString::compileDic( const char* lexFile ) {
	initConstruction();
	
	std::ifstream fileHandle( lexFile );
	if( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" + 
					     std::string( lexFile ) + 
					     "' for reading." );
	}

	
	struct stat f_stat;
	stat( lexFile, &f_stat );
	size_t estimatedNrOfKeys = f_stat.st_size / 10;
	std::cerr<<"Estimate about "<< estimatedNrOfKeys << " Keys."<< std::endl;
	
	annHash_ = new Hash< uchar >( estimatedNrOfKeys, annStrings_, sizeOfAnnStrings_ );


	uchar bytesIn[Global::lengthOfLongStr];
	// set the last byte to 0. So we can recognize when an overlong string was read by getline().
	bytesIn[Global::lengthOfLongStr - 1] = 0; 

	wchar_t key[Global::lengthOfLongStr];
	uchar* annotationStr = 0;

	while( fileHandle.getline(( char* ) bytesIn, Global::lengthOfLongStr ) )  {
 	    if ( bytesIn[Global::lengthOfLongStr-1] != 0 ) {
		throw exceptions::badInput( "csl::MinDicString::compileDic: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    
	    /////////////////// PARSE THE INPUT STRING
	    uchar *c;
	    c = ( uchar* )strchr( ( char* )bytesIn, keyValueDelimiter_ );
	    
	    if( c ) {
		*c = 0;
		annotationStr = ( c + 1 );
	    }
	    else throw exceptions::badInput( "csl::MinDicString::compileDic: No string annotation given." );

	    if( mbstowcs( key, (const char*)bytesIn, Global::lengthOfLongStr ) == (size_t)-1 ) {
		throw exceptions::badInput( "csl::MinDicString::compileDic: Invalid utf-8 sequence" );
	    }

	    if( ! ( c  && *key ) ) {
		throw exceptions::badInput( "csl::MinDicString::compileDic: wrong input format" );
	    }

	    size_t offset = annHash_->findOrInsert( annotationStr );
	    
	    MinDic_t::addToken( key, offset );

	}
	fileHandle.close();

	finishConstruction();
    }

    void MinDicString::printDic() const {
	count_ = 0;
	printDic_rec( getRoot(), 0, 0 );
    }

    void MinDicString::printDic_rec( int pos, int depth, size_t perfHashValue ) const {
	int newPos;
	static wchar_t w[Global::lengthOfStr];
	size_t newPerfHashValue;
	
	const wchar_t* transitions = getSusoString( pos );
	while( *transitions ) {
	    newPerfHashValue = perfHashValue;;
	    if( ( newPos = walkPerfHash( pos, *transitions, &newPerfHashValue ) ) ) {
		w[depth] = *transitions;
		
		if( isFinal( newPos ) ) {
		    w[depth+1] = 0;
		    printf( "%ls#%s\n", w, annStrings_ + getAnnotation( newPerfHashValue ) );
		    //printf( "%ls#%d\n", w, newPerfHashValue );
		    
		    if( ( ++count_ % 100000 ) == 0 ) fprintf( stderr, "%d\n", (int)count_ );
		} // if isFinal
		printDic_rec( newPos, depth + 1, newPerfHashValue );

	    } // if couldWalk
	    else {
		throw exceptions::badDictFile( "suso-string seems to be corrupted." ); 
	    }
	    ++transitions;
	} // while
    } // end of method
}

#endif
