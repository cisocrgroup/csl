namespace csl {

    inline ErrDic::ErrDic( char const* errDicFile ) :
	originals_( 0 ),
	sizeOfOriginals_( 0 ),
	errorPatterns_( 0 ),
	sizeOfErrorPatterns_( 0 ),
	originalHash_( 0 ),
	patternHash_( 0 ),
	keyValueDelimiter_( Global::keyValueDelimiter )
    {
	if( errDicFile ) {
	    loadFromFile( errDicFile );
	}
    }

    inline ErrDic::~ErrDic() {
	if( originals_ ) delete( originals_ );
	if( errorPatterns_ ) delete( errorPatterns_ );
    }

    inline bool ErrDic::lookup( wchar_t const* str, int* annotation ) const {
	throw exceptions::cslException( "csl::ErrDic::lookup: Not yet implemented" );
    }

    inline bool ErrDic::lookup( std::wstring const& str, int* annotation ) const {
	throw exceptions::cslException( "csl::ErrDic::lookup: Not yet implemented" );
    }


    inline bool ErrDic::lookup( const wchar_t* key, Entry* entry ) const {
	size_t tokenIndex;
	if( TransTable_t::getTokenIndex( key, &tokenIndex ) ) {
	    *entry = Entry( *this, tokenIndex );
	    return true;
	}
	else return false;
    }

    inline bool ErrDic::query( std::wstring const& key, Interpretation* answer ) const {
	Entry entry;
	if( lookup( key.c_str(), &entry ) ) {
	    answer->clear();
	    answer->setWord( key );
	    answer->setBaseWord( entry.getOriginal() );

	    std::wstring patString( entry.getErrorPattern() );
	    size_t delimiter = patString.find_first_of( ' ' );
	    if( delimiter == std::wstring::npos ) {
		throw exceptions::cslException( "csl::ErrDic: Bad pattern format in ErrDic" );
	    }
	    answer->getInstruction().push_back( PosPattern( patString.substr( 0, delimiter ), patString.substr( delimiter + 1 ), 42 ) );

	    return true;
	}
	else return false;
    }    


    inline void ErrDic::loadFromFile( const char* dicFile ) {
	FILE* fi = fopen( dicFile, "rb" );
	if ( !fi ) {
	    throw exceptions::badFileHandle( "csl::ErrDic: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for reading." );
	}
	loadFromStream( fi );
	fclose( fi );
    }
    
    inline void ErrDic::loadFromStream( FILE* fi ) {
	fread( &header_, sizeof( Header ), 1, fi );
	if ( ( header_.getMagicNumber() != magicNumber_ ) ) {
	    // throw exceptions::badDictFile( "ErrDic: Magic number comparison failed.\n" );
	}

	sizeOfOriginals_ = header_.getSizeOfOriginals();
	sizeOfErrorPatterns_ = header_.getSizeOfErrorPatterns();

	MinDic_t::loadFromStream( fi );
	originals_ = (wchar_t*) malloc( sizeOfOriginals_ * sizeof( wchar_t ) );
	fread( originals_, sizeof( wchar_t ), sizeOfOriginals_, fi );

	errorPatterns_ = (wchar_t*) malloc( sizeOfErrorPatterns_ * sizeof( wchar_t ) );
	fread( errorPatterns_, sizeof( wchar_t ), sizeOfErrorPatterns_, fi );
    }

    inline void ErrDic::writeToFile( const char* dicFile ) const {
	FILE* fo = fopen( dicFile, "wb" );
	if ( !fo ) {
	    throw exceptions::badFileHandle( "ErrDic: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for writing." );
	}
	writeToStream( fo );
	fclose( fo );
    }
    
    inline void ErrDic::writeToStream( FILE* fo ) const {
	fwrite( &header_, sizeof( Header ), 1, fo );
	MinDic_t::writeToStream( fo );
	fwrite( originals_, sizeof( wchar_t ), header_.getSizeOfOriginals(), fo );
	fwrite( errorPatterns_, sizeof( wchar_t ), header_.getSizeOfErrorPatterns(), fo );
    }

    inline void ErrDic::initConstruction() {
	MinDic_t::initConstruction();
	
	sizeOfOriginals_ = 0;
	sizeOfErrorPatterns_ = 0;

	originalHash_ =  new Hash< wchar_t >( 100000, originals_, sizeOfOriginals_ );
	patternHash_ =  new Hash< wchar_t >( 20000, errorPatterns_, sizeOfErrorPatterns_ );

    }

    inline void ErrDic::finishConstruction() {
	MinDic_t::finishConstruction();
	header_.set( *this );

	delete( originalHash_ );
	delete( patternHash_ );
    }

    inline void ErrDic::compileDic( const char* lexFile ) {
	
	std::wifstream fileHandle( lexFile );
	if( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" + 
					     std::string( lexFile ) + 
					     "' for reading." );
	}

	
	initConstruction();

	std::wstring line;
	std::wstring key;
	wchar_t* original = 0;
	wchar_t* errorPattern = 0;

	while( getline( fileHandle, line ) )  {
 	    if ( line.length()  > Global::lengthOfLongStr ) { // not sure if this is still necessary
		throw exceptions::badInput( "csl::ErrDic::compileDic: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    

	    /////////////////// PARSE THE INPUT STRING
	    wchar_t* c; // go through the string with this pointer

	    // cut off the key

	    size_t endOfKey = line.find_first_of( Global::keyValueDelimiter );
	    if( endOfKey == line.npos ) throw exceptions::badInput( "csl::ErrDic::compileDic: Input format violated." );

	    // cut off the original
	    size_t endOfOriginal = line.find_first_of( L' ', endOfKey + 1 );
	    if( endOfOriginal == line.npos ) throw exceptions::badInput( "csl::ErrDic::compileDic: Input format violated." );

	    // cut off the error pattern
	    size_t insideThePattern = line.find_first_of( L' ', endOfOriginal + 1 );
	    if( insideThePattern == line.npos ) throw exceptions::badInput( "csl::ErrDic::compileDic: Input format violated." );
	    size_t endOfPattern = line.find_first_of( L' ', insideThePattern + 1 );
	    if( endOfPattern == line.npos ) throw exceptions::badInput( "csl::ErrDic::compileDic: Input format violated." );
	    
	    addToken( line.substr( 0, endOfKey ).c_str(),
		      line.substr( endOfKey + 1, endOfOriginal - endOfKey - 1 ).c_str(),
		      line.substr( endOfOriginal + 1, endOfPattern - endOfOriginal - 1 ).c_str()
		);
	    
	}
	if( errno == EILSEQ ) { // catch encoding error
	    throw exceptions::badInput( "MinDic::compileDic: Encoding error in input sequence." );
	}

	fileHandle.close();

	finishConstruction();
    }

    void ErrDic::addToken( const wchar_t* key, const wchar_t* original, const wchar_t* errorPattern ) {
	size_t originalOffset = originalHash_->findOrInsert( original );
	size_t patternOffset = patternHash_->findOrInsert( errorPattern );
	    

//	    printf( "Insert entry: key=%ls, original=%ls, pattern=%ls\n", key, original, errorPattern );
	MinDic_t::addToken( key, MdAnnType( originalOffset, patternOffset ) );
    }

    void ErrDic::printDic() const {
	count_ = 0;
	printDic_rec( getRootState(), 0 );
    }

    void ErrDic::printDic_rec( State const& pos, int depth ) const {
	State newPos( *this );
	static wchar_t w[Global::lengthOfStr];
	size_t newPerfHashValue;
	
	const wchar_t* transitions = pos.getSusoString();
	while( *transitions ) {
	    if( ( newPos = pos.getTransTarget( *transitions ) ).isValid() ) {
		w[depth] = *transitions;
		
		if( newPos.isFinal() ) {
		    w[depth+1] = 0;
		    assert( newPos.getAnnotation().second < sizeOfErrorPatterns_ );

		    wprintf( L"%ls#%ls\n", w, errorPatterns_ + newPos.getAnnotation().second );
		    //printf( "%ls#%d\n", w, newPerfHashValue );

		    if( ( ++count_ % 100000 ) == 0 ) fprintf( stderr, "%d\n", (int)count_ );
		} // if isFinal
		printDic_rec( newPos, depth + 1 );

	    } // if couldWalk
	    else {
		throw exceptions::badDictFile( "suso-string seems to be corrupted." ); 
	    }
	    ++transitions;
	} // while
    } // end of method


    inline void ErrDic::doAnalysis() const {
	float originals_MB = (float)( header_.getSizeOfOriginals() * sizeof( wchar_t ) ) / 1000000;
	float errorPatterns_MB = (float)( header_.getSizeOfErrorPatterns() * sizeof( wchar_t ) ) / 1000000;

	MinDic_t::doAnalysis();
	printf( "**********\nErrDic Analysis\n**********\noriginals-buffer:\t%.3f MB\npattern-buffer:\t%.3f MB\n\n",
		originals_MB,
		errorPatterns_MB
	    );
    }

} // namespace csl
