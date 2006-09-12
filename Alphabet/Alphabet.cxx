#ifndef CSL_ALPHABET_CXX
#define CSL_ALPHABET_CXX CSL_ALPHABET_CXX

#include<iostream>
#include "./Alphabet.h"


namespace csl {

    Alphabet::Alphabet( char* alphFile ) {

	memset( code_, 0, 256 );
	memset( decode_, 0, 257 );

	uchar* alphStr = new uchar[300];

	// read and process alphabet file
	FILE* alph_handle = fopen( alphFile, "r" );
	if ( ! alph_handle ) {
	    throw exceptions::badFileHandle( "csl::Alphabet: Couldn't open alphabet file: " + std::string( alphFile ) );
	}
	fgets( ( char* )alphStr, 300, alph_handle );
	fclose( alph_handle );

	if ( strlen( ( char* )alphStr ) > 256 ) {
	    throw exceptions::badInput( "csl::Alphabet: More than 256 characters in alphabet string." );
	}

	// this loop fills the array 'code' and sets 'size_' to the size
	// of the alphabet as specified in the alphabet file
	size_ = 0;

	uchar* c = alphStr;
	while ( *c ) {
	    ++size_;
	    code_[*c] = size_;
	    decode_[size_] = *c;
	    ++c;
	}

    }



    int Alphabet::strcmp( const uchar* a, const uchar* b ) const {
	int z;
	while ( ( z = ( code( *a ) - code( *b ) ) ) == 0 && *a ) {

	    if ( code( *a ) == 0 && !( *a == 10 ) ) { //if unknown char, not newline
		std::cerr << "Alphabet: Invalid character '" << *a << "'(" << ( int )*a << ")" << std::endl;
		exit( 1 );
	    }
	    if ( code( *b ) == 0 && !( *b == 10 ) ) { //if unknown char, not newline
		std::cerr << "Alphabet: Invalid character '" << *b << "'(" << ( int )*b << ")" << std::endl;
        exit( 1 );
      }

      ++a;
      ++b;
    }
    if ( !( *a ) ) return 0;
    else return z / abs( z );

  }

} // end of namespace csl

#endif
