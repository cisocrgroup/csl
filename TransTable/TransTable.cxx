#ifndef CSL_TRANSTABLE_CXX
#define CSL_TRANSTABLE_CXX CSL_TRANSTABLE_CXX

#include "./TransTable.h"

namespace csl {

    template< CellType CellTypeValue >
    TransTable< CellTypeValue >::TransTable( size_t alphSize ) : magicNumber_( 46388436  ) {
	alphSize_ = alphSize;
	
	cells_ = 0;
	susoStrings_ = 0;
	ftHash_ = 0;
    }

    template< CellType CellTypeValue >
    TransTable< CellTypeValue >::~TransTable() {
	// if( cells_ ) free( cells_ );
	// if( susoStrings_ ) free( susoStrings_ );
	// header ?
    }

    template< CellType CellTypeValue >
    inline int TransTable< CellTypeValue >::walk( int state, uchar c ) const {
	assert( cells_[state].isOfType( Cell_t::STATE ) );
	return ( cells_[state + c].getKey() == c ) ? cells_[state + c].getValue() : 0;
    }

    template<>
    inline int TransTable< TOKDIC >::walkPerfHash( uint_t state, uchar c, size_t& perfHashValue ) const {
	assert( cells_[state].isOfType( Cell_t::STATE ) );
	if( cells_[state + c].getKey() == c ) {
	    perfHashValue += cells_[state + c].getAddByte();
	    return cells_[state + c].getValue();
	}
	return 0;
    }

    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::initConstruction() {
	nrOfCells_ = 0;
	allocCells( 80 );
	nrOfStates_ = 0;
	firstFreeCell_ = 1; // cells_[0] is never used
	sizeOfUsedCells_ = 1; // cells_[0] is never used

	header_.magicNumber_ = magicNumber_;
	header_.cType_ = CellTypeValue;
	header_.offsetCells_ = sizeof( Header );
	header_.offsetSusoStrings_ = 0;
	header_.lengthOfSusoStrings_ = 0;
	susoStrings_ = 0;

	if ( CellTypeValue == TOKDIC ) {
	    ftHash_ = new Hash( 100000, susoStrings_, header_.lengthOfSusoStrings_ );
	}
    }

    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::finishConstruction() {
	header_.nrOfCells_ = sizeOfUsedCells_;

	if ( CellTypeValue == TOKDIC ) { // not very nice
	    header_.offsetSusoStrings_ = header_.offsetCells_ + ( sizeOfUsedCells_ * sizeof( Cell_t ) );
	    header_.lengthOfSusoStrings_ = ftHash_->getLengthOfKeyStrings();
	    delete( ftHash_ );
	    ftHash_ = 0;
	}
    }

    
    template< CellType CellTypeValue >
    bool TransTable< CellTypeValue >::loadFromFile( const char* binFile ) {
	FILE * fi;

	std::cerr << "TransTable: Reading " << binFile << " ... " << std::flush;
	fi = fopen( binFile, "rb" );
	if ( !fi ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" +
					     std::string( binFile ) +
					     "' for reading." );
	}

	loadFromFile( fi );
	fclose( fi );
	std::cerr<< "Ok"<<std::endl;
	return true;
    }

    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::loadFromFile( FILE* fi ) {
	fread( &header_, sizeof( Header ), 1, fi );

	if ( ( header_.magicNumber_ != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "TransTable: Magic number comparison failed.\n" );
	}

	if ( header_.cType_ != CellTypeValue ) {
	    std::cerr << header_.cType_ << "<->" << CellTypeValue << std::endl;
	    throw exceptions::badDictFile( "csl::TransTable - File is incompatible to dictionary type.\n" );
	}

	cells_ = (Cell_t*) malloc( header_.nrOfCells_ * sizeof( Cell_t ) );
	fread( cells_, sizeof( Cell_t ), header_.nrOfCells_, fi );

	susoStrings_ = (uchar*) malloc( header_.lengthOfSusoStrings_ * sizeof( uchar ) );
	fread( susoStrings_, sizeof( uchar ), header_.lengthOfSusoStrings_, fi );
	
	sizeOfUsedCells_ = nrOfCells_ = header_.nrOfCells_;
    }
    


    /**
     * resize the array of cells
     */
    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::allocCells( int newNrOfCells ) {
	cells_ = ( Cell_t* ) realloc( cells_, newNrOfCells * sizeof( Cell_t ) );
	memset( ( cells_ + nrOfCells_ ), 0, ( newNrOfCells - nrOfCells_ ) * sizeof( Cell_t ) );
	nrOfCells_ = newNrOfCells;
// std::cout<<"Resized array cells to "<<newNrOfCells<<" cells."<<std::endl;
    }


    /**
     * find slot to store state
     */
    template<>
    inline int TransTable< BASIC >::findSlot( const TempState_t& state ) {
	size_t slot = firstFreeCell_ - 1; // is incremented again at beginning of loop
	size_t i;
	size_t nrOfAnnotations = state.getNrOfAnnotations();

	bool mightFit = false;
	while ( !mightFit ) {
	    do {
		++slot;
		while ( ( slot + nrOfAnnotations + alphSize_ + 1 ) > nrOfCells_ ) { // okay???
		    allocCells( nrOfCells_ * 2 );
		}
	    } while ( ! ( cells_[slot].isEmpty() ) );

	    mightFit = true;

	    // check if cells for annotations are available;
	    // increment slot with the loop, so that slot points to the cell that gets type STATE
	    for ( i = 1; ( mightFit && i < nrOfAnnotations ) ;++i ) {
		if ( !cells_[++slot].isEmpty() )
		    mightFit = false;
	    }

	    // check if all required cells for transitions are available
	    for ( i = 1; ( mightFit && i <= alphSize_ ) ; ++i ) {
		if ( state.getTransTarget( i ) && ( !cells_[slot + i].isEmpty() ) ) {
		    mightFit = false;
		}
	    }
	}
	return slot;
    }

    /**
     * find slot to store state
     */
    template<>
    inline int TransTable< TOKDIC >::findSlot( const TempState_t& state ) {
	size_t slot = firstFreeCell_ - 1; // is incremented again at beginning of loop
	size_t i;
	size_t nrOfAnnotations = state.getNrOfAnnotations();

	bool mightFit = false;
	while ( !mightFit ) {
	    do {
		++slot;
		while ( ( slot + nrOfAnnotations + alphSize_ + 1 ) > nrOfCells_ ) { // okay???
		    allocCells( nrOfCells_ * 2 );
		}
	    } while ( ! ( cells_[slot].isEmpty() ) );

	    mightFit = true;

	    // check if cells for annotations are available;
	    // increment slot with the loop, so that slot points to the cell that gets type STATE
	    for ( i = 1; ( mightFit && i < nrOfAnnotations ) ;++i ) {
		if ( !cells_[++slot].isEmpty() )
		    mightFit = false;
	    }

	    // check if all required cells for transitions are available
	    for ( const uchar* c = state.getTransString(); mightFit && *c; ++c ) {
		assert( state.getTransTarget( *c ) );
		if ( ! cells_[slot + *c].isEmpty() ) {
		    mightFit = false;
		}
	    }
	}
	return slot;
    }

    template< CellType CellTypeValue >
    int TransTable< CellTypeValue >::storeTempState( TempState_t& state ) {
	return 0; // dummy
    }

    template<>
    inline int TransTable< BASIC >::storeTempState( TempState_t& state ) {
	++nrOfStates_;

	size_t nrOfAnnotations = state.getNrOfAnnotations();

	size_t slot = findSlot( state );
	size_t i;


	// mark stateCell and insert the first annotation
	cells_[slot].setType( Cell_t::STATE );
	if ( nrOfAnnotations > 0 ) {
	    cells_[slot].setType( Cell_t::HAS_ANN );
	    cells_[slot].setValue( state.getAnnotation( 0 ) );
	}

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal( true );
	}

	// insert further annotations
	for ( i = 1; i < nrOfAnnotations ;++i ) {
	    cells_[slot - i].setType( Cell_t::IS_ANN );
	    cells_[slot - i].setValue( state.getAnnotation( i ) );
	}

	// insert all transitions
	for ( i = 1; i <= alphSize_ ; ++i ) {
	    if ( state.getTransTarget( i ) ) {
		cells_[slot + i].setTrans( i, state.getTransTarget( i ) );
	    }
	}

	// update nrOfUsedCells
	sizeOfUsedCells_ = std::max( sizeOfUsedCells_, ( slot + alphSize_ + 2 ) );

	//update firstFreeCell
	for ( ;! ( cells_[firstFreeCell_].isEmpty() );++firstFreeCell_ );


	return slot;
    }



    template<>
    inline int TransTable< TOKDIC >::storeTempState( TempState_t& state ) {
	++nrOfStates_;

	// add offset for susoString as value
	size_t susoPtr = ftHash_->findOrInsert( state.getTransString() );
	state.addAnnotation( susoPtr );

	int slot = findSlot( state );

	// mark stateCell and insert the only annotation
	cells_[slot].setType( Cell_t::STATE );
	cells_[slot].setType( Cell_t::HAS_ANN );
	cells_[slot].setValue( state.getAnnotation() );

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal( true );
	}

	// insert all transitions
	// insert all transitions
	for ( const uchar* c = state.getTransString(); *c; ++c ) {
	    assert( state.getTransTarget( *c ) );
	    cells_[slot + *c].setTrans( *c, state.getTransTarget( *c ), state.getTransPhValue( *c ) );
	}
	
	// update sizeOfUsedCells_
	sizeOfUsedCells_ = std::max( sizeOfUsedCells_, ( slot + alphSize_ + 2 ) );

	// the following lines are to prevent firstFreeCell_ to get stuck in a lower area, where none of the
	// new states can never fit.
	if( ( slot - firstFreeCell_ ) > 2000 ) {
//	    std::cerr << "ffc hack: " << firstFreeCell_ << std::endl;
	    firstFreeCell_ = slot - 2000; // a slot which is really empty is searched right below
	}

	//update firstFreeCell
	for ( ; ! ( cells_[firstFreeCell_].isEmpty() ); ++firstFreeCell_ );


	return slot;
    }

    template< CellType CellTypeValue  >
    void TransTable< CellTypeValue >::createBinary( char* compFile ) {
	FILE * fo = fopen( compFile, "wb" );
	if ( !fo ) {
	    std::cerr << "Couldn't open " << compFile << std::endl;
	    exit( 1 );
	}

	std::cerr << "Dumping automaton to " << compFile << " ... ";
	writeToFile( fo );
	fclose( fo );
	std::cerr << "Ok" << std::endl;
    }

    template< CellType CellTypeValue  >
    void TransTable< CellTypeValue >::writeToFile( FILE* fo ) const {
	if ( !fo ) {
	    throw exceptions::badFileHandle( "TransTable: Couldn't write to filehandle " );
	}
	std::cerr << "Writing TransTable"<<std::endl;
	fwrite( &header_, sizeof( Header ), 1, fo );
	fwrite( cells_, sizeof( Cell_t ), sizeOfUsedCells_, fo );
	
	fwrite( susoStrings_, sizeof( uchar ), header_.lengthOfSusoStrings_, fo );
    }

    template< CellType CellTypeValue  >
    size_t TransTable< CellTypeValue >::getSizeOnDisk() const {
	return ( 
	    sizeof( Header ) +
	    ( sizeof( Cell_t ) * sizeOfUsedCells_ ) + 
	    ( sizeof( uchar ) * header_.lengthOfSusoStrings_ )
	    );
    }

    template<>
    inline bool TransTable< TOKDIC >::compareStates( const TempState_t& temp, int comp ) const {
	if ( temp.isFinal() != isFinal( comp ) ) return false;
	for ( const uchar* c = temp.getTransString(); *c; ++c ) {
	    assert( temp.getTransTarget( *c ) );
	    if ( temp.getTransTarget( *c ) != walk( comp, *c ) ) return false;
	}
	return true;
    }

    /**
     * compare a temporary state to a compressed one.
     * return iff both are equivalent w.r.t their outgoing transitions
     */
    template< CellType CellTypeValue  >
    inline bool TransTable< CellTypeValue >::compareStates( const TempState_t& temp, int comp ) const {
	if ( temp.isFinal() != isFinal( comp ) ) return false;
	for ( size_t c = 1; c <= alphSize_; ++c ) {
	    if ( temp.getTransTarget( c ) != walk( comp, c ) ) return false;
	}
	return true;
    }


    template< CellType CellTypeValue  >
    void TransTable< CellTypeValue >::printCells() const {
	std::cout << "i\ttype\tkey\tvalue" << std::endl << "--------------" << std::endl;
	for ( size_t i = 0; i < nrOfCells_; ++i ) {
	    std::cout << i << "\t" << cells_[i].getType() << "\t" << cells_[i].getKey() << "\t" << cells_[i].getValue() << std::endl;
	}
    }

    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::toDot( const Alphabet* alph ) const {
	Cell_t * cellArray = TransTable::getCells();
	std::cout << "Digraph TransTable_out { //DOTCODE" << std::endl << "rankdir=LR; //DOTCODE" << std::endl;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cellArray[i].isOfType( Cell_t::TRANS ) ) {
		int base = i - cellArray[i].getKey();
		// depending on whether a pointer to an Alphabet-object was given,
		// print either the coded integer or the decoded character
		std::cout << base << "->" << cellArray[i].getValue() << "[label=";
		if ( alph ) std::cout << ( char ) alph->decode( cellArray[i].getKey() );
		else std::cout << cellArray[i].getKey();
		std::cout << "] //DOTCODE" << std::endl;
	    } else if ( cellArray[i].isOfType( Cell_t::STATE ) ) {
		int peripheries = ( cellArray[i].isOfType( Cell_t::FINALSTATE ) ) ? 2 : 1;
		std::cout << i << "[peripheries=" << peripheries << "] //DOTCODE" << std::endl;

		if ( cellArray[i].isOfType( Cell_t::HAS_ANN ) ) {
		    std::cout << i << "->" << i << "[label=\"[";

		    AnnIterator it( *this, i );
		    while ( it.isValid() ) {
			std::cout << *it << ",";
			++it;
		    }
		    std::cout << "]\",fontcolor=blue,style=dotted,dir=none] //DOTCODE" << std::endl;
		    std::cout << std::endl;
		}
	    }
	}
	std::cout << "} //DOTCODE" << std::endl;
    }

    template<>
    inline void TransTable< TOKDIC >::toDot( const Alphabet* alph ) const {
	Cell_t * cellArray = TransTable::getCells();
	std::cout << "Digraph TransTable_out { //DOTCODE" << std::endl << "rankdir=LR; //DOTCODE" << std::endl;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cellArray[i].isOfType( Cell_t::TRANS ) ) {
		int base = i - cellArray[i].getKey();
		// depending on whether a pointer to an Alphabet-object was given,
		// print either the coded integer or the decoded character
		std::cout << base << "->" << cellArray[i].getValue() << "[";
		if ( alph ) std::cout << "label=\"" << ( char ) alph->decode( cellArray[i].getKey() );
		else std::cout << "label=\"" << cellArray[i].getKey();
		std::cout << "(" << cellArray[i].getAddByte() << ")\"";
		std::cout << "] //DOTCODE" << std::endl;
	    } else if ( cellArray[i].isOfType( Cell_t::STATE ) ) {
		int peripheries = ( cellArray[i].isOfType( Cell_t::FINALSTATE ) ) ? 2 : 1;
		printf( "%d[label=\"%d\", peripheries=%d] //DOTCODE\n", i, i, peripheries );
		
		if ( cellArray[i].isOfType( Cell_t::HAS_ANN ) ) {}
	    }
	}
	std::cout << "} //DOTCODE" << std::endl;
    }
}

#endif
