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

	    TransTable(int alphSize, char* binFile = NULL);
	    ~TransTable();

	    /**
	     * Loads an automaton from a binary file. (usually .dic)
	     * @param binFile filename of the binary storing the automaton
	     */
	    bool loadBinary(const char* binFile);
    
	    /**
	     * Dumps the automaton into a file
	     * @param binFile filename to dump the automaton into (usually .dic)
	     */
	    void createBinary(char* binFile);

	    /**
	     * Call this method before adding the first state to the automaton
	     */
	    void initConstruction();

	    /**
	     * Call this method to finish the construction phase
	     */
	    void finishConstruction();

	    /**
	     * @param newTempState
	     * inserts a new state into the table, returns the state id.
	     */ 
	    int storeTempState(TempState_t& state);
	
	    /**
	     * declare a root (or start state) of the automaton.
	    */
	    void setRoot(int rootId) {
		cells_[0].setValue(rootId);
	    }
	    
	    /// returns the root
	    int getRoot() const {
		return cells_[0].getValue();
	    }
	
	    /**
	     * returns true iff state is marked as final
	     * @param state id of a state
	     */
	    bool isFinal(int state) const {
		assert(cells_[state].isOfType(Cell_t::STATE));
		return cells_[state].isOfType(Cell_t::FINALSTATE);
	    }
	
	    void setFinal(int state, bool b = true) {
		// assertion that state is really a state happens in Cell's setFinal()
		cells_[state].setFinal(true);
	    }
	
	    bool hasAnnotations(int state) const {
		assert(cells_[state].isOfType(Cell_t::STATE));
		return cells_[state].isOfType(Cell_t::HAS_ANN);
	    }

	    int getFirstAnn(int state) const {
//	    assert(hasAnnotations(state));
		return cells_[state].getValue();
	    }

	    /**
	     * @return the string containing labels of all existing outgoing transitions from state
	     */
	    const uchar* getSusoString( int state ) const {
		return (susoStrings_ + getFirstAnn( state ) );
	    }

	    int getNrOfCells() const {
		return nrOfCells_;
	    }

	    /**
	       the most important function: perform one step inside the graph
	       @param state the state to start from
	       @param c the character to walk with (c already being coded according to custom alphabet)
	    */
	    int walk(int state, uchar c) const {
		assert(cells_[state].isOfType(Cell_t::STATE)); // leave this assertion out for additional speed
		return (cells_[state + c].getKey() == c)? cells_[state + c].getValue() : 0;
	    }

	
	    /**
	       This method allows to change the target of a transition of a state which
	       is already compiled. This transition has to have been present beforehands, only 
	       its target value is changed (see assertion).
	       The method is necessary for TransTable to store an Aho-Corasick-Automaton: The error links
	       can only be processed for a complete trie.(see class csl::AhoCorasick)
	    */
	    void changeTransitionTarget(int state, int c, int newValue) {
		assert(cells_[state + c].getKey() == c);
		cells_[state + c].setValue(newValue);
	    }

	    class AnnIterator {
		private:
		const int state_; ///< stores the state-cell
		int cell_; ///< iterates through the annotation cells
		Cell_t* cells_; ///< pointer to the sparse table
		public:
		AnnIterator(const TransTable& tt, int state) : state_(state) {
		    cells_ = tt.getCells();
		    assert(cells_[state_].isOfType(Cell_t::STATE));
		    cell_ = state_;
		}
		AnnIterator& operator++() {
		    --cell_;
		    return *this;
		}
		int operator*() {
		    assert(isValid());
		    return cells_[cell_].getValue();
		}

		bool isValid() {
		    // the iterator is 'valid' if it points to a IS_ANN cell or 
		    //                         if it points to its state_-cell which carries annotation
		    return (cells_[cell_].isOfType(Cell_t::IS_ANN) || ((cell_==state_) && cells_[cell_].isOfType(Cell_t::HAS_ANN)));
		}
	    };

	    class TransIterator {
	    public:
		TransIterator( const uchar* susoString ) {
		    pos_ = susoString;
		}
		uchar operator* () {
		    return *pos_;
		}
		TransIterator& operator++() {
		    ++pos_;
		    return *this;
		}
		
		bool isValid() {
		    return ( *pos_ != 0 );
		}
	    private:
		const uchar* pos_;
	    };

	    TransIterator getTransIterator( uint_t state_id ) const {
		return TransIterator( getSusoString( state_id ) );
	    }

	    // getters
	    Cell_t* getCells() const {
		return cells_;
	    }

	    int getNrOfCells() {
		return nrOfCells_;
	    }
	
	    int getAlphSize() const {
		return alphSize_;
	    }
	
	    bool compareStates(const TempState_t& temp, int comp) const;

	    void printCells() const;
	    void toDot(const Alphabet* alph = NULL) const;



	    private:
	    int alphSize_;
	    Cell_t* cells_;
	    int nrOfCells_;

	    uchar* susoStrings_;
	    size_t lengthOfSusoStrings_;
	    Hash* ftHash_;

	    // to be set in the constructor. We need a terminating \0 her as 9th byte
	    const bits64 magicNumber_;
	    typedef struct {
		bits64 magicNumber_;
		CellType cType_;
		size_t offsetCells_;
		size_t offsetSusoStrings_;
	    } Header;

	    Header* header_;

	    /**
	       The number of states. Note that this value is updated during construction
	       only. If a table is loaded from a binary, this variable remains set to -1.
	    */
	    int nrOfStates_;

	    /// the first free cell of the array
	    int firstFreeCell_;
	    int sizeOfUsedCells_;


	    /**
	       resize the array of cells
	    */
	    void allocCells(int newNrOfCells);

	    int findSlot(const TempState_t& state);

	};

} // eon

#include "./TransTable.cxx"


#endif
