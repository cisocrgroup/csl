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
    private:
	/**
	 * CellIndex should be 8 bytes whehever 
	 * StateId_t is 8 bytes
	 */
	typedef StateId_t CellIndex;
    public:
	typedef Cell< CellTypeValue, StateId_t > Cell_t;

	/**
	 *
	 */
	inline TransTable();

	/**
	 * Destructor
	 */
	inline ~TransTable();

	const Alphabet& getAlphabet() const;

	/**
	 * Loads an automaton from a binary file. (usually .dic or .trie or similar)
	 * @param binFile filename of the binary storing the automaton
	 * @throw exceptions::badFileHandle
	 */
	inline bool loadFromFile( const char* binFile );

	inline void loadFromStream( FILE* fi );

	/**
	 * Dumps the automaton into a file
	 * @param binFile filename to dump the automaton into (usually .dic)
	 */
	inline void createBinary( char* binFile );

	/**
	 * 
	 */
	void writeToStream( FILE* fo ) const;


	/**
	 * Call this method before adding the first state to the automaton
	 */
	inline void initConstruction();

	/**
	 * Call this method to finish the construction phase
	 */
	inline void finishConstruction();

	/**
	 * inserts a new state into the table, returns the state id.

	 * @param state a TempState-object
	 * @returns the state id
	 */
	inline StateId_t storeTempState( TempState& state );

	/**
	 * declare a root (or start state) of the automaton.
	 */
	inline void setRoot( StateId_t rootId ) {
	    cells_[0].setValue( rootId );
	}

	/// returns the root
	inline StateId_t getRoot() const {
	    return cells_[0].getValue();
	}

	/**
	 * returns true iff state is marked as final
	 * @param state id of a state
	 */
	inline bool isFinal( StateId_t state ) const {
	    assert( cells_[state].isOfType( Cell_t::STATE ) );
	    return cells_[state].isOfType( Cell_t::FINALSTATE );
	}

	inline void setFinal( StateId_t state, bool b = true ) {
	    // assertion that state is really a state happens in Cell's setFinal()
	    cells_[state].setFinal( true );
	}

	inline bool hasAnnotations( StateId_t state ) const {
	    assert( cells_[state].isOfType( Cell_t::STATE ) );
	    return cells_[state].isOfType( Cell_t::HAS_ANN );
	}

	inline int getFirstAnn( StateId_t state ) const {
//     assert(hasAnnotations(state));
	    return cells_[state].getValue();
	}

	/**
	 * @return the string containing labels of all existing outgoing transitions from state
	 */
	inline const wchar_t* getSusoString( StateId_t state ) const {
	    return ( susoStrings_ + getFirstAnn( state ) );
	}


	/**
	 * perform one step inside the automaton
	 * @param state the state to start from
	 * @param c the character to walk with
	 * @returns the state id reached by the transition. 0, if the transition does not exist
	 */
	inline uint_t walk( StateId_t state, char16 c ) const;

	/**
	 * 
	 */
	inline uint_t walkStr( StateId_t state, const wchar_t* str ) const {
	    while( *str && state ) {
		state = walk( state, *str );
		++str;
	    }
	    return state;
	}

	/**
	 * perform one step inside the automaton and keep track of the perfect hashing value
	 * Implemented for TOKDIC only
	 * @param state the state to start from
	 * @param c the character to walk with (c already being coded according to custom alphabet)
	 * @param perfHashValue the perfHash value of the current traansition is ADDED to this variable
	 */
	inline StateId_t walkPerfHash( StateId_t state, char16 c, size_t& perfHashValue ) const;

	inline StateId_t walkStrPerfHash( StateId_t state, const wchar_t* str, size_t& perfHashValue ) const;

	inline bool getTokID( const wchar_t* key, size_t* tokID ) const;


	class AnnIterator {
	private:
	    const int state_; ///< stores the state-cell
	    CellIndex cell_; ///< iterates through the annotation cells
	    Cell_t* cells_; ///< pointer to the sparse table
	public:
	    AnnIterator( const TransTable& tt, StateId_t state ) : state_( state ) {
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
	    TransIterator( const wchar_t* susoString ) {
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

	    const wchar_t* getPointer() const {
		return pos_;
	    }

	private:
	    const wchar_t* pos_;
	};

	TransIterator getTransIterator( StateId_t state_id ) const {
	    return TransIterator( getSusoString( state_id ) );
	}

	// getters
	inline Cell_t* getCells() const {
	    return cells_;
	}

	inline int getNrOfCells() {
	    return nrOfCells_;
	}

	Alphabet& getAlphabet() {
	    return alph_;
	}

	const Alphabet& getConstAlphabet() const {
	    return alph_;
	}
	
	inline bool compareStates( const TempState& temp, StateId_t comp ) const;

	inline size_t count( StateId_t pos ) const;
	inline void printCells() const;
	inline void toDot() const;
	inline void doAnalysis() const;


    protected:
	inline size_t getNrOfCells() const {
	    return nrOfCells_;
	}

	inline size_t getSizeOfUsedCells() const {
	    return sizeOfUsedCells_;
	}

	inline size_t getNrOfStates() const {
	    return nrOfStates_;
	}

    private:
	/**
	 * This number specifies the maximum size of the area that is considered when
	 * looking for a slot. Free cells 
	 */
	static const size_t searchWindow = 1000;

	Alphabet alph_;
	
	Cell_t* cells_;
	size_t nrOfCells_;

	wchar_t* susoStrings_;

	// this is the number of characters in susoStrings_ (not the size in bytes!)
	size_t lengthOfSusoStrings_;
	Hash<wchar_t>* susoHash_;

	static const bits64 magicNumber_ = 46388436;

	/**
	 * This class represents the header information for a TransTable. An object of this kind
	 * is dumped to/ read from the stream with fread/fwrite. The policy is to have all that
	 * information in separate variables in TransTable and to copy the values to and from the header
	 * just for dumping and loading
	 */
	class Header {
	public:
	    Header() :
		magicNumber_( 0 ),
		cType_( 0 ),
		nrOfCells_( 0 ),
		lengthOfSusoStrings_( 0 ) {
	    }

	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }

	    int getCType() const {
		return cType_;
	    }

	    size_t getNrOfCells() const {
		return nrOfCells_;
	    }
	    size_t getLengthOfSusoStrings() const {
		return lengthOfSusoStrings_;
	    }

	    void set( const TransTable< CellTypeValue >& transTable ) {
		magicNumber_ = transTable.magicNumber_;
		cType_ = CellTypeValue;
		nrOfCells_ = transTable.nrOfCells_;
		lengthOfSusoStrings_ = transTable.lengthOfSusoStrings_;
	    }
	    

	private:
	    bits64 magicNumber_;
	    bits64 cType_;
	    bits64 nrOfCells_;
	    bits64 lengthOfSusoStrings_;
	};

	Header header_;

	/**
	   The number of states. Note that this value is updated during construction
	   only. If a table is loaded from a binary, this variable remains set to -1.
	*/
	size_t nrOfStates_;

	/// the first free cell of the array
	size_t firstFreeCell_;
	size_t sizeOfUsedCells_;


	/**
	 * resize the array of cells
	 */
	inline void allocCells( size_t newNrOfCells );

	inline StateId_t findSlot( const TempState& state );


  };

} // eon

#include "./TransTable.cxx"


#endif
