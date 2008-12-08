#ifndef CSL_GLOBAL_H
#define CSL_GLOBAL_H CSL_GLOBAL_H

#include<cassert>
#include<cstring>
#include<string>
#include<exception>
#include<ios>
#define debug(s) (std::cout<<"DEBUG: "<<s<<std::endl)


#ifdef WIN32
#pragma warning(disable:4996) // wcsncpy deprecated
#pragma warning(disable:4251) // strange warning about std::vector

#define CSL_DECLSPEC __declspec(dllexport)
#else
#define CSL_DECLSPEC 
#endif

#if defined(_MSC_VER)
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif 



/**
   This namespace covers a system of modules for string processing.
   @todo 
   - rename typedefs to "fasel_t"
*/
namespace csl {
    typedef unsigned short char16;
    typedef wchar_t char32;

    typedef unsigned char uchar;

    typedef unsigned int uint_t;

    typedef int bits32;

    typedef unsigned long long ulong_t;
    typedef unsigned long long bits64;

    typedef unsigned int StateId_t;

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

    namespace Global {
       
#ifdef CSL_ROOTDIR	
	static const std::string cslRootDir( CSL_ROOTDIR );
#endif

	static const size_t maxNrOfChars = 65536;

	// those lengths are the maximum size of bytes from the input stream
	static const size_t lengthOfWord = 64;
	static const size_t lengthOfStr = 3000;
	static const size_t lengthOfLongStr = 5000;

	static const int LevDEA_maxDistance = 3;

	static const wchar_t keyValueDelimiter = '#';
	static const uchar valuesDelimiter = ',';

	// For more results, the program should not break but do a (maybe not so efficient) realloc
	static const int LevMaxNrOfResults = 200;

	class Perm {
	public:
	    static const int tokenDelimiter = ',';
	    static const int permuteDelimiter = '%';
	    static const int noPermuteDelimiter = '$';
	    static const size_t maxNrOfTokens = 32;
	    static const size_t maxCandsPerToken = 200;
	    static const size_t maxNrOfResults = 100;
	};


	static const void reverse(const wchar_t* str, wchar_t* newStr) {
	    size_t len = wcslen( str );
	    size_t i = 0;
	    while(i < len) {
		newStr[len-1-i] = str[i];
		++i;
	    }
	    newStr[len] = 0;
	}

	/**
	 * write the reversed \c str to \c newStr
	 * @param[in] str some string
	 * @param[out] the resukt is written to this string
	 */
	static const void reverse( const std::wstring& str, std::wstring* newStr ) {
	    if( &str == newStr ) {
		// throw exceptions::cslException( "Global::reverse: source string may not be equal to target string" );
	    }
	    size_t len = str.length();
	    newStr->resize( len );
	    size_t i = 0;
	    while(i < len) {
			newStr->at( len-1-i ) = str.at( i );
			++i;
	    }
	}

	static const void printBits( bits64 n ) {
	    int i;
	    for ( i = 63;i >= 0;--i ) {
		if ( ( i % 10 ) == 0 ) printf( "%d", i / 10 );
		else if ( ( i % 10 ) == 5 ) printf( "|" );
		else printf( " " );
	    }
	    printf( "\n" );
	    for ( i = 63;i >= 0;--i ) {
		printf( "%u", ( unsigned int )( 1 & ( n >> i ) ) );
	    }
	    printf( "\n" );
	}
	
    }

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

	class LogicalError : public cslException {
	public:
	    LogicalError() {}
	    LogicalError( const std::string msg ) : cslException( msg ) {}
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
