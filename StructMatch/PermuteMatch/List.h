class List : public LevFilter::ResultSet_if  { // of class PermuteMatch

public:

    class Item {
    public:
	/**
	 * @todo should the strcmp-value be inverted?
	 */
	bool operator < ( const Item& other ) {
	    int ret = 0;
	    ret = other.getValue() - getValue();
	    if( ret == 0 ) {
		ret = getCol() - other.getCol();
		if( ret == 0 ) {
		    ret = strcmp( (char*)getStr(), (char*)other.getStr() );
		}
	    }
	    return ( ret > 0 );
	}

	void set( const uchar* str, int value, int col ) {
	    strcpy( ( char* )str_, ( char* )str );
	    value_ = value;
	    colNr_ = col;
	    colBit_ = 1ll << colNr_;
	}
	const uchar* getStr() const {
	    return str_;
	}
	int getValue() const {
	    return value_;
	}

	bits32 getStillPossible() {
	    return stillPossible_;
	}
	void setStillPossible( bits32 v ) {
	    stillPossible_ = v;
	}

	size_t getCol() const {
	    return colNr_;
	}

	bits32 getColBit() {
	    return colBit_;
	}

    private: // of Item
	uchar str_[Global::lengthOfWord];
	int value_;
	bits32 colBit_;
	size_t colNr_;
	bits32 stillPossible_;
    }; // class Item

    List() {
	reset();
    }

    Item& at_merged( size_t i ) {
	assert( i < size_merged_ );
	return *list_merged_[i];
    }
    const Item& at_merged( size_t i ) const {
	assert( i < size_merged_ );
	return *list_merged_[i];
    }

    Item& at_sep( size_t token, size_t i ) {
	assert( i < size_sep_[token] );
	return lists_sep_[token][i];
    }
    const Item& at_sep( size_t token, size_t i ) const {
	assert( i < size_sep_[token] );
	return lists_sep_[token][i];
    }


    static bool is_equal( const Item& a, const Item& b ) {
	return( ( a.getValue() == b.getValue() ) && ( a.getCol() == b.getCol() ) &&  ( strcmp( ( char* )a.getStr(), ( char* )b.getStr() ) == 0 ) );
    }

    static bool cmp( const Item& a, const Item& b ) {
	return ( ( a.getValue() < b.getValue() ) || // let freq() decide
		 ( ( a.getValue() == b.getValue() )  // or if freqs are equal
		   && ( strcmp( ( char* )a.getStr(), ( char* )b.getStr() ) < 0 ) ) ); // let alphab. order decide (standard strcmp function, not the one implemented in Alphabet)
    }

    void sort() {
	sort( 0, getSize_merged() - 1 ); // private method
    }

    void sortUnique( size_t token ) {
	std::sort( lists_sep_[token], lists_sep_[token] + size_sep_[token], cmp );
	size_sep_[token]= std::unique( lists_sep_[token], lists_sep_[token] + size_sep_[token] ) - lists_sep_[token] ;
    }

    size_t getSize_merged() const {
	return size_merged_;
    }

    size_t getSize_sep( size_t token ) const {
	return size_sep_[token];
    }

    void setCurCol( int col ) {
	curCol_ = col;
    }

    void push( const uchar* str, int value ) {
	if ( size_sep_[curCol_] > ( Global::Perm::maxCandsPerToken - 1 ) ) {
	    throw exceptions::bufferOverflow( "ResultSet: ResultSet overflow." );
	} 
	else {
	    at_sep( curCol_, size_sep_[curCol_] ).set( str, value, curCol_ );
	    ++( size_sep_[curCol_] );
	    
	    list_merged_[size_merged_] = &( at_sep( curCol_, size_merged_ ) );
	    ++size_merged_;
	}
    }

    void reset() {
	size_merged_ = 0;
	for( size_t i; i < Global::Perm::maxNrOfTokens; size_sep_[i] = 0, ++i );
    }

    void calcStillPossible() {
	bits32 n = 0;
	if( getSize_merged() == 0 ) return;
	size_t i = getSize_merged();
	while( i > 0 ) {
	    --i;
	    n |= at_merged( i ).getCol();
	    at_merged( i ).setStillPossible( n );
	}
    }

private: // of List
    Item lists_sep_[Global::Perm::maxNrOfTokens][Global::Perm::maxCandsPerToken];
    size_t size_sep_[Global::Perm::maxNrOfTokens];

    Item* list_merged_[Global::Perm::maxNrOfTokens * Global::Perm::maxCandsPerToken];
    size_t size_merged_;

    int curCol_; ///< used during filling the list


    // quicksort: see Cormen, Introduction to algorithms, p.145ff
    void sort( size_t left, size_t right ) {
	size_t q;
	if ( left < right ) {
	    q = partition( left, right );
	    sort( left, q - 1 );
	    sort( q + 1, right );
	}
    }

    // for quicksort
    int partition( size_t left, size_t right ) {
	size_t i = left - 1;
	size_t j;
	for ( j = left; j < right; ++j ) {
	    if ( at_merged(j) <  at_merged(right) ) {
		++i;
		swap( i, j );
	    }
	}
	swap( i + 1, right );
	return ( i + 1 );
    }

    void swap( size_t i, size_t j ) {
	Item tmp = at_merged(i);
	at_merged(i) = at_merged( j );
	at_merged(j) = tmp;
    }

public:
  void printList()  {
      printf( "%20s\t%16s\t%s\t%s\n", "str", "col", "val", "sp" );
      for ( size_t i = 0; i < getSize_merged(); ++i ) {
	  printf( "%20s\t%16d\t%d\t%d\n", at_merged( i ).getStr(), at_merged( i ).getColBit(), at_merged( i ).getValue(), at_merged( i ).getStillPossible() );
      }
  }

};
