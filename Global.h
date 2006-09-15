#ifndef CSL_GLOBAL_H
#define CSL_GLOBAL_H CSL_GLOBAL_H

#include <cassert>
#include <cstring>
#include <string>
#include <exception>
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
	/// just 4 bytes per transition (currently not implemented)
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

    typedef enum { 
	STANDARD,
	FW_BW
    } MSMatchMode;



    class Global {
    public:
	static const int maxNrOfChars = 256;

	static const int lengthOfWord = 64;
	static const int lengthOfStr = 3000;
	static const int lengthOfLongStr = 5000;

	static const int LevDEA_maxDistance = 3;

	static const uchar keyValueDelimiter = '#';
	static const uchar valuesDelimiter = ',';

	static const int LevMaxNrOfResults = 25000;

	class Perm {
	public:
	    static const int tokenDelimiter = ',';
	    static const int permuteDelimiter = '%';
	    static const int noPermuteDelimiter = '$';
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

    namespace exceptions {

	class cslException : public std::exception {
	public:
	    cslException() {
	    }

	    cslException( const std::string& msg ) : msg_( msg ) {
	    }

	    virtual ~cslException() throw() {}
	    
	    virtual const char* what() const throw() {
		return msg_.c_str();
	    }
	    
	private:
	    const std::string msg_;
	};

	class AlphabetError : public cslException {
	public:
	    AlphabetError() {}
	    AlphabetError( const std::string msg ) : cslException( msg ) {}
	};

	class badInput : public cslException {
	public:
	    badInput() {}
	    badInput( const std::string msg ) : cslException( msg ) {}
	};

	class badDictFile : public cslException {
	public:
	    badDictFile() {}
	    badDictFile( const std::string msg ) : cslException( msg ) {}
	};

	class badFileHandle : public cslException {
	public:
	    badFileHandle() {}
	    badFileHandle( const std::string msg ) : cslException( msg ) {}
	};

	class invalidLevDistance : public cslException {
	public:
	    invalidLevDistance() {}
	    invalidLevDistance( const std::string msg ) : cslException( msg ) {}
	};

	class bufferOverflow : public cslException {
	public:
	    bufferOverflow() {}
	    bufferOverflow( const std::string msg ) : cslException( msg ) {}
	};

	class outOfRange : public cslException {
	public:
	    outOfRange() {}
	    outOfRange( const std::string msg ) : cslException( msg ) {}
	};

    };



} // eon


#endif
