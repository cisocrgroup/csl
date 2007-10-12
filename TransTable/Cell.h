#ifndef CSL_TRANSTABLE_CELL_H
#define CSL_TRANSTABLE_CELL_H CSL_TRANSTABLE_CELL_H

/**
   Represents one unit of the sparse table implementation in TransTable  
   \author Uli Reffle <uli@reffle.de>
   \date Jan 2005
*/

#include <cassert>
#include <iostream>
#include "../Global.h"
namespace csl {

    template< CellType C, class StateId_t >
    class Cell {
	// dummy class
	// we have only specialisations
    };


    /**
     * This specialisation is not at all maintained at the moment !!!
     */
    template< class StateId_t >
    class Cell< BASIC, StateId_t > { // of class TransTable
    public:

	/* type is bit-coded: 
	    1 = 00001: transition
	    2 = 00010: state
	    4 = 00100: state that has annotation attached
	    8 = 01000: cell that stores annotation
	   16 = 10000: is a final state
	*/
	typedef short CellType;
	enum {TRANS=1,STATE=2,IS_ANN=4,HAS_ANN=8,FINALSTATE=16};

	Cell() :
	    type_( 0 ),
	    key_( 0 ),
	    value_( 0 ) {
	}

	void setKey(int key) {
	    assert( key_ == 0 );
	    assert( key < 65536 );
	    key_ = key;
	}
	wchar_t getKey() const {
	    return key_;
	}


	void setValue( StateId_t value ) {
//	assert(value_ == 0); // can not be asserted when changeTransitionTarget() is used
	    value_ = value;
	}

	StateId_t getValue() const {
	    return value_;
	}

	void setTrans( int key, StateId_t value ) {
	    assert(value_ == 0);
	    setKey(key);
	    setValue(value);
	    setType(TRANS);
	}

	void setType(CellType type) {
	    type_ |=  type;
	}

	void setFinal(bool f = true) {
	    assert(isOfType(Cell::STATE));
	    setType(Cell::FINALSTATE);
	}

	bool isEmpty() const {
	    return (type_ == 0);
	}

	bool isOfType(CellType t) const {
	    return ((type_ & t) != 0);
	}

	CellType getType() const {
	    return type_;
	}

	void print() const {
	    std::cout<<"k="<<getKey()<<", v="<<getValue()<<std::endl;
	}


    private:
	short type_;

	short key_;
	int value_;
	
    };



    template< class StateId_t >
    class Cell< TOKDIC, StateId_t > { // of class TransTable
    public:

	/* type is bit-coded: 
	   1 = 00001: transition
	   2 = 00010: state
	   4 = 00100: state that has annotation attached
	   8 = 01000: cell that stores annotation
	   16 = 10000: is a final state
	*/
	typedef short CellType;
	enum {TRANS=1,STATE=2,IS_ANN=4,HAS_ANN=8,FINALSTATE=16};

	Cell() : type_(0),
		 key_(0),
		 value_(0),
		 addByte_(0) {
	    // do nothing else
	}

	void setKey(wchar_t key) {
	    assert(key_ == 0);
	    assert( key < 65536 );
	    key_ = key;
	}

	wchar_t getKey() const {
	    return key_;
	}

	void setValue(int value) {
//	assert(value_ == 0); // can not be asserted when changeTransitionTarget() is used
	    value_ = value;
	}
	StateId_t getValue() const {
	    return value_;
	}

	void setAddByte(int addByte) {
	    assert(addByte_ == 0);
	    addByte_ = addByte;
	}

	int getAddByte() const {
	    return addByte_;
	}

	void setTrans(int key, int value, int addByte) {
	    assert(value_ == 0);
	    setKey(key);
	    setValue(value);
	    setAddByte(addByte);
	    setType(TRANS);
	}

	void setType(CellType type) {
	    type_ |=  type;
	}

	void setFinal(bool f = true) {
	    assert(isOfType(Cell::STATE));
	    setType(Cell::FINALSTATE);
	}

	bool isEmpty() const {
	    return (type_ == 0);
	}

	bool isOfType(CellType t) const {
	    return ((type_ & t) != 0);
	}

	CellType getType() const {
	    return type_;
	}

	void print() const {
	    std::cout<<"k="<<getKey()<<", v="<<getValue()<<std::endl;
	}


    private:
	short type_;
	short key_;

	StateId_t value_;
	int addByte_;
    };

} // eon

#endif
