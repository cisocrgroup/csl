#ifndef RESULTSET_H
#define RESULTSET_H RESULTSET_H

#include<cassert>
#include<iostream>
#include<vector>
#include<algorithm>
#include "../Alphabet/Alphabet.h"
#include "../Global.h"
#include "../LevFilter/LevFilter.h"

namespace csl {

    /**
     * A container class for candidate sets extracted by a LevFilter object
     * @author Uli Reffle
     * @date 2006
     */
    class ResultSet : public LevFilter::ResultSet_if {
    public:
	ResultSet( const Alphabet& alph ) : 
	    alph_( alph ) {
	    reset();
	}

	/**
	 * returns the current size of the ResultSet
	 *
	 * @return the current size of the ResultSet
	 */
	int getSize() const {
	    return size_;
	}

	/**
	 * add another item to the ResultSet
	 */
	void push( const uchar* str, int annotation ) {
	    if ( size_ > ( Global::LevMaxNrOfResults - 1 ) ) {
		throw exceptions::bufferOverflow( "ResultSet: ResultSet overflow." );
	    }
	    else {
		listBuffer_[size_].set( str, annotation );
		list_[size_] = &listBuffer_[size_];
		++size_;
	    }
	}

	/**
	 * delete all items from the list
	 */
	void reset() {
	    size_ = 0;
	    
	}

	class Item {
	private:
	    uchar str_[Global::lengthOfWord];
	    int annotation_;
	public:
	    bool operator==( const Item& other ) const {
		return !( strcmp( (char*)getStr(), (char*)other.getStr() ) );
	    }
	    void set( const uchar* str, int ann ) {
		strcpy( ( char* )str_, ( char* )str );
		annotation_ = ann;
	    }
	    const uchar* getStr() const {
		return str_;
	    }
	    int getAnn() const {
		return annotation_;
	    }
	}; // class Item


	const Item& operator[]( int i ) const {
	    assert( i < size_ );
	    return *( list_[i] );
	}


	/**
	 * @attention the strcmp is the standard one, not the one that depends on the Alphabet-object
	 */
	static bool cmp( const Item* a, const Item* b ) {
	    return strcmp( (char*)a->getStr(), (char*)b->getStr() ) < 0; 
	}

	static bool is_equal( const Item* a, const Item* b ) {
	    return( strcmp( (char*)a->getStr(), (char*)b->getStr() ) == 0 ); 
	}

	void sort() {
	    std::sort( list_, list_ + size_, cmp );
	}

	void sortUnique() {
	    sort();
	    size_ = std::unique( list_, list_ + size_, is_equal ) - list_;
	}
	
    private:
	const Alphabet& alph_;
	int size_;
	Item* list_[Global::LevMaxNrOfResults];
	Item listBuffer_[Global::LevMaxNrOfResults];
    };

  /// provide a nice print for a ResultSet::Item
  std::ostream& operator<<( std::ostream& os, const csl::ResultSet::Item& item );


} // eon


#endif
