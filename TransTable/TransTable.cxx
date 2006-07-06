#ifndef CSL_TRANSTABLE_CXX
#define CSL_TRANSTABLE_CXX CSL_TRANSTABLE_CXX

#include "./TransTable.h"

namespace csl {


    template< CellType CellTypeValue >
    TransTable< CellTypeValue >::TransTable ( int alphSize, char* binFile ) : magicNumber_ ( 46388436  ) {
	alphSize_ = alphSize;

	cells_ = 0;
	susoStrings_ = 0;
	ftHash_ = 0;

	if ( binFile ) { // binary file was given
	    nrOfCells_ = -1;
	    loadBinary ( binFile );
	}
    }

    template< CellType CellTypeValue >
    TransTable< CellTypeValue >::~TransTable() {
	if ( cells_ ) free ( cells_ );
	if ( susoStrings_ ) free ( susoStrings_ );
    }

/**
 * @remarks 
 *   There is some code-duplication here, so remember
 *   to make all changes also in the general template 
 *   for this method
 */
    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::initConstruction() {
	nrOfCells_ = 0;
	allocCells ( 80 );
	nrOfStates_ = 0;
	firstFreeCell_ = 1; // cells_[0] is never used
	sizeOfUsedCells_ = 1; // cells_[0] is never used
	header_ = new Header();
	header_->magicNumber_ = 17; //magicNumber_;
	header_->cType_ = TOKDIC;
	header_->offsetCells_ = sizeof ( Header );
	header_->offsetSusoStrings_ = 0;
	susoStrings_ = 0;

	if ( CellTypeValue == TOKDIC ) {
	    lengthOfSusoStrings_ = 0;
	    ftHash_ = new Hash ( 10000, susoStrings_, lengthOfSusoStrings_ );
	}
    }

    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::finishConstruction() {
	if ( CellTypeValue == TOKDIC ) {
	    header_->offsetSusoStrings_ = sizeOfUsedCells_ * sizeof ( Cell_t );
	    lengthOfSusoStrings_ = ftHash_->getLengthOfKeyStrings();
	    delete ( ftHash_ );
	    delete ( header_ );
	}
    }
    
    template< CellType CellTypeValue >
    bool TransTable<CellTypeValue>::loadBinary ( const char* binFile ) {
	FILE * fi;
	struct stat f_stat;

	std::cerr << "TransTable: Reading " << binFile << " ... " << std::flush;
	stat ( binFile, &f_stat );
	fi = fopen ( binFile, "rb" );
	if ( !fi ) {
	    std::cerr << "TransTable: Could not read binary file: " << binFile << std::endl;
	    exit ( 1 );
	}

	nrOfCells_ = ( int ) ( f_stat.st_size / sizeof ( Cell_t ) );
	sizeOfUsedCells_ = nrOfCells_;
	uchar* file_ = (uchar*) malloc ( f_stat.st_size );
	fread ( file_, 1, f_stat.st_size, fi );
	fclose ( fi );

	Header* header = (Header*) file_;
	if( header->magicNumber_ != magicNumber_ ) {
	    printf( "Magic number comparison failed.\n" );
	    return false;
	}
	if( header->cType_ != CellTypeValue ) {
	    printf( "CellType comparison failed.\n" );
	    return false;
	}
	cells_ = (Cell_t*) ( file_ + header->offsetCells_ );
	susoStrings_ = (uchar*) ( file_ + header->offsetSusoStrings_ );

	std::cerr << "Ok" << std::endl;

	return true;
    }


    /**
     * resize the array of cells
     */
    template< CellType CellTypeValue >
    void TransTable< CellTypeValue >::allocCells ( int newNrOfCells ) {
	cells_ = ( Cell_t* ) realloc ( cells_, newNrOfCells * sizeof ( Cell_t ) );
	memset ( ( cells_ + nrOfCells_ ), 0, ( newNrOfCells - nrOfCells_ ) * sizeof ( Cell_t ) );
	nrOfCells_ = newNrOfCells;
// std::cout<<"Resized array cells to "<<newNrOfCells<<" cells."<<std::endl;
    }


    template< CellType CellTypeValue >
    int TransTable< CellTypeValue >::storeTempState ( TempState_t& state ) {
	return 0; // dummy
    }

    template<>
    int TransTable< BASIC >::storeTempState ( TempState_t& state ) {
	++nrOfStates_;

	int nrOfAnnotations = state.getNrOfAnnotations();

	int slot = findSlot ( state );
	int i;


	// mark stateCell and insert the first annotation
	cells_[slot].setType ( Cell_t::STATE );
	if ( nrOfAnnotations > 0 ) {
	    cells_[slot].setType ( Cell_t::HAS_ANN );
	    cells_[slot].setValue ( state.getAnnotation ( 0 ) );
	}

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal ( true );
	}

	// insert further annotations
	for ( i = 1; i < nrOfAnnotations ;++i ) {
	    cells_[slot - i].setType ( Cell_t::IS_ANN );
	    cells_[slot - i].setValue ( state.getAnnotation ( i ) );
	}

	// insert all transitions
	for ( i = 1; i <= alphSize_ ; ++i ) {
	    if ( state.getTransTarget ( i ) ) {
		cells_[slot + i].setTrans ( i, state.getTransTarget ( i ) );
	    }
	}

	// update nrOfUsedCells
	sizeOfUsedCells_ = std::max ( sizeOfUsedCells_, ( slot + alphSize_ + 2 ) );

	// the following lines are to prevent firstFreeCell_ to get stuck in a lower area, where none of the
	// new states can never fit.
	// This hack was necessary for the building of the AhoCorasick-Trie: there, firstFreeCell_ always got
	// stuck in low regions of the compressed table.
	// The value of maximum difference (currently set to 500) works well for AhoCorasick, but does not trigger
	// any skips for a standard Trie.
	if ( ( slot - firstFreeCell_ ) > 500 ) {
	    std::cerr << "ffc hack: " << firstFreeCell_ << std::endl;
	    firstFreeCell_ = slot - 500; // a slot which is really empty is searched right below
	}

	//update firstFreeCell
	for ( ;! ( cells_[firstFreeCell_].isEmpty() );++firstFreeCell_ );


	return slot;
    }



    template<>
    int TransTable< TOKDIC >::storeTempState ( TempState_t& state ) {
	++nrOfStates_;

	// add offset for susoString as value
	size_t susoPtr = ftHash_->findOrInsert ( state.getTransString() );
	state.addAnnotation ( susoPtr );

	int slot = findSlot ( state );

	// mark stateCell and insert the only annotation
	cells_[slot].setType ( Cell_t::STATE );
	cells_[slot].setType ( Cell_t::HAS_ANN );
	cells_[slot].setValue ( state.getAnnotation() );

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal ( true );
	}

	// insert all transitions
	for ( int i = 1; i <= alphSize_ ; ++i ) {
	    if ( state.getTransTarget ( i ) ) {
		cells_[slot + i].setTrans ( i, state.getTransTarget ( i ), state.getTransPhValue ( i ) );
	    }
	}

	// update lastUsedCell
	sizeOfUsedCells_ = std::max ( sizeOfUsedCells_, ( slot + alphSize_ + 2 ) );

	// the following lines are to prevent firstFreeCell_ to get stuck in a lower area, where none of the
	// new states can ever fit.
	// This hack was necessary for the building of the AhoCorasick-Trie: there, firstFreeCell_ always got
	// stuck in low regions of the compressed table.
	// The value of maximum difference (currently set to 500) works well for AhoCorasick, but does not trigger
	// any skips for a standard Trie.
	// By the way, AhoCorasick was thrown out of this library in the meantime. But maybe the hack can be useful
	// in some other situation.
	if ( ( slot - firstFreeCell_ ) > 500 ) {
	    //std::cerr<<"ffc hack: "<<firstFreeCell_<<std::endl;
	    firstFreeCell_ = slot - 500; // a slot which is really empty is searched right below
	}

	//update firstFreeCell
	for ( ; ! ( cells_[firstFreeCell_].isEmpty() ); ++firstFreeCell_ );


	return slot;
    }

    template< CellType CellTypeValue  >
    void TransTable< CellTypeValue >::createBinary ( char* compFile ) {
	FILE * fo = fopen ( compFile, "wb" );
	if ( !fo ) {
	    std::cerr << "Couldn't open " << compFile << std::endl;
	    exit ( 1 );
	}

	std::cerr << "Dumping automaton to " << compFile << " ... ";
	fwrite ( header_, sizeof ( Header ), 1, fo );
	fwrite ( cells_, sizeof ( Cell_t ), sizeOfUsedCells_, fo );
	fwrite ( susoStrings_, sizeof ( uchar ), lengthOfSusoStrings_, fo );
	fclose ( fo );
	std::cerr << "Ok" << std::endl;
    }



/**
   find slot to store state
*/
    template< CellType CellTypeValue >
    int TransTable< CellTypeValue >::findSlot ( const TempState_t& state ) {
	int slot = firstFreeCell_ - 1; // is incremented again at beginning of loop
	int i;
	int nrOfAnnotations = state.getNrOfAnnotations();


	bool mightFit = false;
	while ( !mightFit ) {
	    do {
		++slot;
//  printf("Try at %d\n", slot);
		while ( ( slot + nrOfAnnotations + alphSize_ + 1 ) > nrOfCells_ ) { // okay???
		    allocCells ( nrOfCells_ * 2 );
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
		if ( state.getTransTarget ( i ) && ( !cells_[slot + i].isEmpty() ) ) {
		    mightFit = false;
		}
	    }
	}
	return slot;
    }

/**
   compare a temporary state to a compressed one.
   return iff both are equivalent w.r.t their outgoing transitions
*/
    template< CellType CellTypeValue  >
    bool TransTable< CellTypeValue >::compareStates ( const TempState_t& temp, int comp ) const {
	if ( temp.isFinal() != isFinal ( comp ) ) return false;
	for ( int c = 1;c <= alphSize_;++c ) {
	    if ( temp.getTransTarget ( c ) != walk ( comp, c ) ) return false;
	}
	return true;
    }


    template< CellType CellTypeValue  >
    void TransTable< CellTypeValue >::printCells() const {
	std::cout << "i\ttype\tkey\tvalue" << std::endl << "--------------" << std::endl;
	for ( int i = 0;i < nrOfCells_;++i ) {
	    std::cout << i << "\t" << cells_[i].getType() << "\t" << cells_[i].getKey() << "\t" << cells_[i].getValue() << std::endl;
	}
    }

    template< CellType CellTypeValue  >
    void TransTable< CellTypeValue >::toDot ( const Alphabet* alph ) const {
	Cell_t * cellArray = TransTable::getCells();
	std::cout << "Digraph TransTable_out { //DOTCODE" << std::endl << "rankdir=LR; //DOTCODE" << std::endl;
	for ( int i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cellArray[i].isOfType ( Cell_t::TRANS ) ) {
		int base = i - cellArray[i].getKey();
		// depending on whether a pointer to an Alphabet-object was given,
		// print either the coded integer or the decoded character
		std::cout << base << "->" << cellArray[i].getValue() << "[label=";
		if ( alph ) std::cout << ( char ) alph->decode ( cellArray[i].getKey() );
		else std::cout << cellArray[i].getKey();
		std::cout << "] //DOTCODE" << std::endl;
	    } else if ( cellArray[i].isOfType ( Cell_t::STATE ) ) {
		int peripheries = ( cellArray[i].isOfType ( Cell_t::FINALSTATE ) ) ? 2 : 1;
		std::cout << i << "[peripheries=" << peripheries << "] //DOTCODE" << std::endl;

		if ( cellArray[i].isOfType ( Cell_t::HAS_ANN ) ) {
		    std::cout << i << "->" << i << "[label=\"[";

		    AnnIterator it ( *this, i );
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
    void TransTable< TOKDIC >::toDot ( const Alphabet* alph ) const {
	Cell_t * cellArray = TransTable::getCells();
    std::cout << "Digraph TransTable_out { //DOTCODE" << std::endl << "rankdir=LR; //DOTCODE" << std::endl;
    for ( int i = 1; i < sizeOfUsedCells_; ++i ) {
        if ( cellArray[i].isOfType ( Cell_t::TRANS ) ) {
            int base = i - cellArray[i].getKey();
            // depending on whether a pointer to an Alphabet-object was given,
            // print either the coded integer or the decoded character
            std::cout << base << "->" << cellArray[i].getValue() << "[";
            if ( alph ) std::cout << "label=\"" << ( char ) alph->decode ( cellArray[i].getKey() );
            else std::cout << "label=\"" << cellArray[i].getKey();
            std::cout << "(" << cellArray[i].getAddByte() << ")\"";
            std::cout << "] //DOTCODE" << std::endl;
        } else if ( cellArray[i].isOfType ( Cell_t::STATE ) ) {
            int peripheries = ( cellArray[i].isOfType ( Cell_t::FINALSTATE ) ) ? 2 : 1;
            printf ( "%d[label=\"%d\", peripheries=%d] //DOTCODE\n", i, i, peripheries );

            if ( cellArray[i].isOfType ( Cell_t::HAS_ANN ) ) {}
        }
    }
    std::cout << "} //DOTCODE" << std::endl;
}

}

#endif
