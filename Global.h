#ifndef CSL_GLOBAL_H
#define CSL_GLOBAL_H CSL_GLOBAL_H

#include<cstring>

#define debug(s) (std::cout<<"DEBUG: "<<s<<std::endl)

/**
   This namespace covers a system of modules for string processing.
   @todo 
   - rename typedefs to "fasel_t"
*/
namespace csl {
    typedef unsigned char uchar;

    typedef unsigned int uint_t;
    typedef unsigned int bits32;

    typedef unsigned long long ulong_t;
    typedef unsigned long long bits64;

    typedef enum { 
	/// just 4 bytes per transition (not implemented)
	SLIM, 

	/// 8 bytes per transition, multiple values can be stored inside states
	BASIC,

	/**
	 * 12 bytes per transition; 
	 * perfect hashing (ph) provides offset for a single value, 
	 * "SuperSonic(suso) Mode" provides for each state a string of available transition-labels
	 * designed to hold huge token dictionaries with some frequency (or other) score
	 */
	TOKDIC
    } CellType ;

    class Global {
    public:
	static const int maxNrOfChars = 256;

	static const int lengthOfWord = 64;
	static const int lengthOfStr = 3000;
	static const int lengthOfLongStr = 5000;

	static const int LevDEA_maxDistance = 3;

	static const uchar keyValueDelimiter = '#';
	static const uchar valuesDelimiter = ',';

	static const int LevMaxNrOfResults = 5000;

	class Perm {
	public:
	    static const int tokenDelimiter = ',';
	    static const int componentDelimiter = '$';
	    static const int maxNrOfTokens = 30;
	    static const int maxNrOfResults = 5000;
	    static const int listSize = 60000;
	};


	static const void reverse(const uchar* str, uchar* newStr) {
	    int len = std::strlen((char*)str);
	    int i = 0;
	    while(i < len) {
		newStr[len-1-i] = str[i];
		++i;
	    }
	    newStr[len] = 0;
	}

    };

} // eon


#endif