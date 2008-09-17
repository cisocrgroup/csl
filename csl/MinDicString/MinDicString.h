#ifndef CSL_MINDICSTRING_H
#define CSL_MINDICSTRING_H CSL_MINDICSTRING_H

#include "../Global.h"
#include "../MinDic/MinDic.h"
#include "../Hash/Hash.h"
#include <sys/stat.h>

namespace csl {

    /**
     * This class provides the storage and lookup of huge dictionaries whose entries are annotated
     * with a string-value of arbitrary length.
     *
     * @author Uli Reffle
     * @date 2006
     */
    class MinDicString : public MinDic<> {
    public:
	typedef MinDic<> MinDic_t;
	inline MinDicString();
	inline void setKeyValueDelimiter( uchar c );

	inline const uchar* lookup( wchar_t* key ) const;

	/**
	 * return the string value annotated to the entry with the given perfect hash value
	 */
	inline const uchar* getAnnByPerfHashValue( size_t perfHashValue ) const;

	inline void loadFromFile( char* dicFile );
	inline void loadFromStream( FILE* fi );

	inline void writeToFile( char* dicFile ) const;
	inline void writeToStream( FILE* fo ) const;

	inline void initConstruction();
	inline void finishConstruction();
	inline void compileDic( const char* lexFile );
	inline void printDic() const;

    private:
	inline void printDic_rec( int pos, int depth, size_t perfHashValue ) const;


	static const bits64 magicNumber_ = 2343572;

	class Header {
	public:
	    Header() : 
		magicNumber_( 0 ), 
		sizeOfAnnStrings_( 0 ) {
	    }

	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }
	    size_t getSizeOfAnnStrings() const {
		return sizeOfAnnStrings_;
	    }
	    
	    void set( const MinDicString& mds ) {
		magicNumber_ = mds.magicNumber_;
		sizeOfAnnStrings_ = mds.sizeOfAnnStrings_;
	    }
	private:
	    bits64 magicNumber_;
	    bits64 sizeOfAnnStrings_;
	}; // class Header

	Header header_;

	uchar* annStrings_;
	size_t sizeOfAnnStrings_;

	Hash<uchar>* annHash_;

	uchar keyValueDelimiter_;	

	mutable size_t count_; // is used for counting during printing
    };


} // namespace csl

#include "./MinDicString.tcc"

#endif
