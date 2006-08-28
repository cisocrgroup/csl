#include <iostream>

#ifndef CSL_ALPHABET_H
#define CSL_ALPHABET_H CSL_ALPHABET_H


#include "../Global.h"

namespace csl {

    /**
     * @author Uli Reffle
     * @date 2006
     *
     * 
     * The class Alphabet implements a mapping of ascii-characters to a smaller system 
     * appropriate for the alphabet size that is really needed. 
     * As input it takes as input a file of the form:
     * ---------------file----------------
     * abcdefghijklmnopqrstuvwxyzäöü
     * -----------------------------------
     */
    class Alphabet {
    private:
	size_t size_;
	uchar code_[256];
	// 257, as counting chars starts with 1
	uchar decode_[257];

    public:
	/**
	   The constructor takes as argument a file containing all required characterss.
	*/
	Alphabet( char* alphFile );

	/**
	   Function ascii-Wert --> Custom-Wert
	*/
	inline int code( const uchar c, bool check = true ) const {
	    if( check && code_[c] == 0 && c != 0 ) {
		std::cerr << "Alphabet: Error: Unknown character '" << c << "'" << std::endl;
		exit( 1 );
	    }

	    return code_[c];
	}

	/**
	   Function Custom-Wert --> ascii-Wert
	*/
	inline uint_t decode( const uint_t c ) const {
	    if( c > size() ) {
		std::cerr << "Alphabet: Error: Unknown code: '" << c << "'" << std::endl;
		exit( 1 );
	    }

	    return decode_[c];
	}

	/**
	   Returns the alphabet size
	*/
	inline size_t size() const {
	    return size_;
	}

	/**
	   provides a string compare according to the alphabet order
    */
    int strcmp( const uchar* s1, const uchar* s2 ) const;


  };

} // end of namespace csl
#endif
