#ifndef RESULTSET_H
#define RESULTSET_H RESULTSET_H

#include<cassert>
#include<iostream>
#include<vector>
#include "../Global.h"
#include "../LevFilter/LevFilter.h"
#include "../Hash/Hash.h"

namespace csl {

    /**
     * A container class for candidate sets extracted by a LevFilter object
     * @author Uli Reffle
     * @date 2006
     */
    class ResultSet : public LevFilter::ResultSet_if {
    public:
	ResultSet() : stringBuffer_( 0 ),
		      stringBufferSize_( 0 ),
		      hash_( Global::LevMaxNrOfResults, stringBuffer_, stringBufferSize_ ) {
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
	    if( hash_.find( str ) ) return;
	    if ( size_ > ( Global::LevMaxNrOfResults - 2 ) ) {
		throw exceptions::bufferOverflow( "ResultSet: ResultSet overflow." );
	    }
	    else {
		list_[size_].set( str, annotation );
		++size_;
		hash_.findOrInsert( str );
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

	class Iterator {
	private:
	    const Item *cur_;
	public:

	    Iterator( const Item *init ) : cur_( init ) {}

	    const Item& operator*() const {
		return *cur_;
	    }

	    Iterator operator++() {
		++cur_;
		return *this;
	    }

	    bool operator==( const Iterator& i2 ) const {
		return ( cur_ == i2.cur_ );
	    }

	    bool operator!=( const Iterator& i2 ) const {
		return ( cur_ != i2.cur_ );
	    }

	}; // class Iterator

	const Item& operator[]( int i ) const {
	    assert( i < size_ );
	    return list_[i];
	}


	Iterator begin() const {
	    return Iterator( list_ );
	}

	Iterator end() const {
	    return Iterator( list_ + size_ );
	}

    private:
	int size_;
	Item list_[Global::LevMaxNrOfResults];

	uchar* stringBuffer_;
	size_t stringBufferSize_;
	Hash hash_;
	
  };

  /// provide a nice print for a ResultSet::Item
  std::ostream& operator<<( std::ostream& os, const csl::ResultSet::Item& item );


} // eon


#endif
