#ifndef LEVDEA_CXX
#define LEVDEA_CXX LEVDEA_CXX

#include "./LevDEA.h"

namespace csl {

    /*
      This practice of sharing the tables here is very very dangerous!
      The first instance whose destructor is called will delete the tables
      and leave the other instances with pointers to noone's land!!!
      This only works as long as all instances die at the same time
    */

    
    LevDEA::LevDEA( const Alphabet& alph, int init_k ) : alph_( alph ), k_( 0 ) {
	charvec_ = ( bits64* ) malloc( alph_.size() * sizeof( bits64 ) );
	k_charvecs_ = new bits32[alph_.size() * Global::lengthOfWord];

	tabsLoaded = 0;

	setDistance( init_k );
    }

    LevDEA::~LevDEA() {
	delete( charvec_ );
// delete(tab);
// delete(fin);
    }


    void LevDEA::setDistance( int initK ) {
	k_ = initK;

	// reset stored k_charvecs
 	memset( k_charvecs_, 0, ( alph_.size() * Global::lengthOfWord * sizeof( bits32 ) ) );

	/*
	  In case of k==1, z2k1 would be 111  (==7) ( 2k+1 set bits )
	  z2k2 would be 1111 (==15)  ( 2k+2 set bits )
	*/
	z2k1 = 1ll;
	z2k1 <<=  2 * k_ + 1;
	z2k1--; // a sequence of 2k+1 1-values
	z2k2 = 1ll;
	z2k2 <<= 2 * k_ + 2;
	z2k2--; // a sequence of 2k+2 1-values

	if( ( tabsLoaded & ( 1 << k_ ) ) == 0 ) { // have the tables for k already been loaded???
	    std::ostringstream ss; ss<<k_; // push k_ into a stream to get it as string

//	    std::string table_file = "/mounts/Users/student/uli/implement/csl/trunk/LevDEA/lev" + ss.str() + "data";
	    std::string table_file = "/files/uli/cis/csl/trunk/LevDEA/lev" + ss.str() + "data";

	    FILE * table_handle = fopen( table_file.c_str(), "r" );

	    if( !table_handle ) {
		std::cerr << "Could not read " << table_file << std::endl;
		exit( 1 );
	    }
	    fscanf( table_handle, "%d\n", &k_ );
	    fscanf( table_handle, "%d\n", &( coresetss[k_] ) );
	    coresets = coresetss[k_];

	    tabs[k_] = new table_cell[z2k2*coresets];
	    fins[k_] = ( int* )malloc( ( 2 * k_ + 1 ) * coresets * sizeof( int ) );
	    tab = tabs[k_];
	    fin = fins[k_];

	    int row, col;
	    char c;

	    for ( row = 0; row < z2k2; row++ ) {
		for ( col = 0; col < coresets; ++col ) {
		    fscanf( table_handle, "%d,%d,", &( table( row, col ).target ), &( table( row, col ).move_pattern ) );
		}
		fscanf( table_handle, "%c", &c );
	    }

	    for ( row = 0; row < 2*k_ + 1; row++ ) {
		for ( col = 0; col < coresets; ++col ) {
		    fscanf( table_handle, "%d,", &fin[coresets*row+col] );
		}
		fscanf( table_handle, "%c", &c );
	    }
	    fclose( table_handle );
	    tabsLoaded = tabsLoaded | ( 1 << k_ );
	} else {
	    // tables were loaded some time before. simply assign the pointers.
	    tab = tabs[k_];
	    fin = fins[k_];
	    coresets = coresetss[k_];
	}
    }

    void LevDEA::loadPattern( const uchar* p ) {
	strcpy( ( char* )pattern_, ( char* )p );
	patLength = strlen( ( char* )pattern_ );

	// reset stored k_charvecs
 	memset( k_charvecs_, 0, ( alph_.size() * Global::lengthOfWord * sizeof( bits32 ) ) );

	calcCharvec();
    }

    void LevDEA::calcCharvec() {
	bits64 c;
	int i;
	memset( charvec_, 0, alph_.size() * sizeof( bits64 ) );
	for ( c = z10, i = 0; i < patLength; i++, c >>= 1 ) {
	    charvec_[alph_.code( pattern_[i] )] |= c;
	}
    }

    bits32 LevDEA::calc_k_charvec( uchar c, int i ) const {
	bits64 r;
	// after the next line, the bits i,i+1,i+2 of chv are the lowest bits of r. All other bits of r are 0
	r = ( charvec_[c] >> ( 64 - ( 2 * k_ + 1 + i ) ) ) & z2k1;
	if ( patLength - i < 2 * k_ + 1 ) // the last few chars of the word
	    r = ( ( r >> ( 2 * k_ + 1 - ( patLength - i ) ) ) | ( zff << ( ( patLength - i ) + 1 ) ) ) & z2k2;
 	
	return ( k_charvecs_[(c * alph_.size() ) + i] = (bits32) r );
    }

    void LevDEA::printTable() const {
	int row, col;
	for ( row = 0; row < z2k2; row++ ) {
	    printf( "%d\t", row );
	    for ( col = 0; col < coresets; ++col ) {
		printf( "(%d,%d)\t", table( row, col ).target, table( row, col ).move_pattern );
	    }
	    printf( "\n" );
	}
    }

    void LevDEA::printCharvec() const {
	uint_t c;
	std::cout << "-------------" << std::endl;
	for( c = 1; c <= alph_.size(); ++c ) {
      if( charvec_[c] ) {
        std::cout << c << std::endl;
        printBits( charvec_[c] );
      }
    }
    std::cout << "-------------" << std::endl;
  }

  void LevDEA::printBits( const bits64& n ) const {
    int i;
    for( i = 63;i >= 0;--i ) {
      if( ( i % 10 ) == 0 ) printf( "%d", i / 10 );
      else if( ( i % 10 ) == 5 ) printf( "|" );
      else printf( " " );
    }
    printf( "\n" );
    for( i = 63;i >= 0;--i ) {
      printf( "%i", ( int )( 1 & ( n >> i ) ) );
    }
    printf( "\n" );

  }

} // eon
#endif
