class List : public LevFilter::ResultSet_if  { // of class PermuteMatch

public:
    class Item {
    private:
	uchar str_[Global::lengthOfWord];
	int value_;
	bits32 colBit_;
	size_t colNr_;
	bits32 stillPossible_;


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
    }; // class Item


    size_t size_;
    Item list_[Global::Perm::listSize];
    int curCol_; ///< used during filling the list

    void calcStillPossible() {
	bits32 n = 0;
	if( getSize() == 0 ) return;
	size_t i = getSize();
	while( i > 0 ) {
	    --i;
	    n |= at( i ).getCol();
	    at( i ).setStillPossible( n );
	}
    }
    
public:
    List() {
	reset();
    }

    Item& operator[]( int i ) {
	return list_[i];
    }

    Item& at( int i ) {
//	assert( i < size_ );
	return list_[i];
    }
    const Item& at( int i ) const {
//	assert( i < size_ );
	return list_[i];
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
	sort( 0, getSize() - 1 ); // private method
    }

    void sortUnique() {
	sort();
	size_ = std::unique( list_, list_ + size_, is_equal ) - list_ ;
	printf("size:%d\n", size_ );
    }

    size_t getSize() const {
	return size_;
    }

    void setCurCol( int col ) {
	curCol_ = col;
    }

    void push( const uchar* str, int value ) {
	if ( size_ > ( Global::Perm::listSize - 1 ) ) {
	    throw exceptions::bufferOverflow( "ResultSet: ResultSet overflow." );
	} else {
	    at( size_ ).set( str, value, curCol_ );
	    ++size_;
	}
    }

    void reset() {
	size_ = 0;
    }


private:
    // quicksort: see Cormen, Introduction to algorithms, p.145ff
    void sort( int left, int right ) {
	int q;
	if ( left < right ) {
	    q = partition( left, right );
	    sort( left, q - 1 );
	    sort( q + 1, right );
	}
    }

    // for quicksort
    int partition( int left, int right ) {
	int i = left - 1;
	int j;
	for ( j = left;j < right;++j ) {
	    if ( at( j ) <  at( right ) ) {
		++i;
		swap( i, j );
	    }
	}
	swap( i + 1, right );
	return ( i + 1 );
    }

    void swap( int i, int j ) {
	Item tmp = at( i );
	at( i ) = at( j );
	at( j ) = tmp;
    }

public:
  void printList()  {
      printf( "%20s\t%16s\t%s\t%s\n", "str", "col", "val", "sp" );
      for ( size_t i = 0; i < getSize(); ++i ) {
	  printf( "%20s\t%16d\t%d\t%d\n", at( i ).getStr(), at( i ).getColBit(), at( i ).getValue(), at( i ).getStillPossible() );
      }
  }

};
