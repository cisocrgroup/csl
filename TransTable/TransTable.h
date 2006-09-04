#ifndef CSL_TRANSTABLE_H
#define CSL_TRANSTABLE_H CSL_TRANSTABLE_H

#include <sys/stat.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <map>
#include<cassert>

#include "../Global.h"
#include "../Alphabet/Alphabet.h"

#include "./Cell.h"
#include "./TempState.h"
#include "../Hash/Hash.h"

namespace csl {

    template< CellType CellTypeValue = BASIC >
    class TransTable {
    public:
	typedef Cell< CellTypeValue > Cell_t;
	typedef TempState< CellTypeValue > TempState_t;

	/**
	 * Constructor
	 * All alphabet information TransTable needs is the number of possible labels. This number
	 * is not necessarily equal to size() of the used Alphabet-object. That's why only a size_type
	 * is passed on rather than an Alphabet-object.
	 * If TransTable is initialized with a binFile, this file is loaded as automaton. 
	 *
	 * @param alphSize the used alphabet size.
	 * @param binFile a binary file conaining an automaton
	 *
	 */
	inline TransTable( size_t alphSize, char* binFile = NULL );

	/**
	 * Destructor
	 */
	inline ~TransTable();

	/**
	 * Loads an automaton from a binary file. (usually .dic)
	 * @param binFile filename of the binary storing the automaton
	 * @throw exceptions::badFileHandle
	 */
	inline bool loadBinary( const char* binFile );

	/**
	 * Dumps the automaton into a file
	 * @param binFile filename to dump the automaton into (usually .dic)
	 */
	inline void createBinary( char* binFile );

	/**
	 * Call this method before adding the first state to the automaton
	 */
	inline void initConstruction();

	/**
	 * Call this method to finish the construction phase
	 */
	inline void finishConstruction();

	/**
	 * @param newTempState
	 * inserts a new state into the table, returns the state id.
	 */
	inline int storeTempState( TempState_t& state );

	/**
	 * declare a root (or start state) of the automaton.
	 */
	inline void setRoot( int rootId ) {
	    cells_[0].setValue( rootId );
	}

	/// returns the root
	inline int getRoot() const {
	    return cells_[0].getValue();
	}

	/**
	 * returns true iff state is marked as final
	 * @param state id of a state
	 */
	inline bool isFinal( int state ) const {
	    assert( cells_[state].isOfType( Cell_t::STATE ) );
	    return cells_[state].isOfType( Cell_t::FINALSTATE );
	}

	inline void setFinal( int state, bool b = true ) {
	    // assertion that state is really a state happens in Cell's setFinal()
	    cells_[state].setFinal( true );
	}

	inline bool hasAnnotations( int state ) const {
	    assert( cells_[state].isOfType( Cell_t::STATE ) );
	    return cells_[state].isOfType( Cell_t::HAS_ANN );
	}

	inline int getFirstAnn( int state ) const {
//     assert(hasAnnotations(state));
	    return cells_[state].getValue();
	}

	/**
	 * @return the string containing labels of all existing outgoing transitions from state
	 */
	inline const uchar* getSusoString( int state ) const {
	    return ( susoStrings_ + getFirstAnn( state ) );
	}

	inline int getNrOfCells() const {
	    return nrOfCells_;
	}

	/**
	   the most important function: perform one step inside the graph
	   @param state the state to start from
	   @param c the character to walk with (c already being coded according to custom alphabet)
	*/
	inline int walk( int state, uchar c ) const {
	    assert( cells_[state].isOfType( Cell_t::STATE ) );
	    return ( cells_[state + c].getKey() == c ) ? cells_[state + c].getValue() : 0;
	}


	/**
	   This method allows to change the target of a transition of a state which
	   is already compiled. This transition has to have been present beforehands, only 
	   its target value is changed (see assertion).
	   The method is necessary for TransTable to store an Aho-Corasick-Automaton: The error links
	   can only be processed for a complete trie.(see class csl::AhoCorasick)
	*/
	inline void changeTransitionTarget( int state, int c, int newValue ) {
	    assert( cells_[state + c].getKey() == c );
	    cells_[state + c].setValue( newValue );
	}

	class AnnIterator {
	private:
	    const int state_; ///< stores the state-cell
	    int cell_; ///< iterates through the annotation cells
	    Cell_t* cells_; ///< pointer to the sparse table
	public:
	    AnnIterator( const TransTable& tt, int state ) : state_( state ) {
		cells_ = tt.getCells();
		assert( cells_[state_].isOfType( Cell_t::STATE ) );
		cell_ = state_;
	    }
	    AnnIterator& operator++() {
		--cell_;
		return *this;
	    }
	    int operator*() const {
		assert( isValid() );
		return cells_[cell_].getValue();
	    }

	    bool isValid() const {
		// the iterator is 'valid' if it points to a IS_ANN cell or
		//                         if it points to its state_-cell which carries annotation
		return ( cells_[cell_].isOfType( Cell_t::IS_ANN ) || 
			 ( ( cell_ == state_ ) && cells_[cell_].isOfType( Cell_t::HAS_ANN ) ) );
	    }
	};

	class TransIterator {
	public:
	    TransIterator( const uchar* susoString ) {
		pos_ = susoString;
	    }
	    uchar operator *() const {
		return *pos_;
	    }
	    TransIterator& operator++() {
		++pos_;
		return *this;
	    }

	    bool isValid() const {
		return ( *pos_ != 0 );
	    }

	    const uchar* getPointer() const {
		return pos_;
	    }

	private:
	    const uchar* pos_;
	};

	TransIterator getTransIterator( uint_t state_id ) const {
	    return TransIterator( getSusoString( state_id ) );
	}

	// getters
	inline Cell_t* getCells() const {
	    return cells_;
	}

	inline int getNrOfCells() {
	    return nrOfCells_;
	}

	inline int getAlphSize() const {
	    return alphSize_;
	}

	inline bool compareStates( const TempState_t& temp, int comp ) const;

	inline void printCells() const;
	inline void toDot( const Alphabet* alph = NULL ) const;



    private:
	size_t alphSize_;
	Cell_t* cells_;
	size_t nrOfCells_;

	uchar* susoStrings_;
	size_t lengthOfSusoStrings_;
	Hash* ftHash_;

	const bits64 magicNumber_;
	typedef struct {
	    bits64 magicNumber_;
	    CellType cType_;
	    size_t offsetCells_;
	    size_t offsetSusoStrings_;
	    size_t nrOfCells_;
	}
	Header;

	Header* header_;

	/**
	   The number of states. Note that this value is updated during construction
	   only. If a table is loaded from a binary, this variable remains set to -1.
	*/
	size_t nrOfStates_;

	/// the first free cell of the array
	size_t firstFreeCell_;
	size_t sizeOfUsedCells_;


	/**
	   resize the array of cells
	*/
	inline void allocCells( int newNrOfCells );

    inline int findSlot( const TempState_t& state );

  };

} // eon

#include "./TransTable.cxx"


#endif
