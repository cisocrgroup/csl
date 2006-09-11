#ifndef CSL_CISLEX_H
#define CSL_CISLEX_H CSL_CISLEX_H

#include "../Global.h"
#include "../Alphabet/Alphabet.h"
#include "../MinDic/MinDic.h"
#include "../Hash/Hash.h"

namespace csl {

    /**
     * This class provides the storage and lookup of CISLEX-entries. 
     * In practice, it is nothing but a dictionary with string-values annotated to each entry.
     *
     * @author Uli Reffle
     * @date 2006
     */

    class Cislex : public MinDic {
    public:
	inline Cislex( const Alphabet& alph );

	inline const uchar* Cislex::lookup( uchar* key ) const;

	inline void loadFromFile( char* dicFile );
	inline void loadFromFile( FILE* fi );

	inline void writeToFile( char* dicFile ) const;
	inline void writeToFile( FILE* fo ) const;

	inline void initConstruction();
	inline void finishConstruction();
	inline void compileDic( const char* lexFile );

    private:
	static const bits64 magicNumber_ = 2343572;

	class Header {
	public:
	    Header() : sizeOfAnnStrings_( 0 ) {}
	    static const bits64 magicNumber_ = 2343572;
	    size_t sizeOfAnnStrings_;
	}; // class Header

	Header header_;

	uchar* annStrings_;
	Hash annHash_;


	
    };

    inline Cislex::Cislex( const Alphabet& alph ) :
	MinDic( alph ),
	annStrings_( 0 ),
	annHash_( 100000, annStrings_, header_.sizeOfAnnStrings_ )
    {
    }

    inline const uchar* Cislex::lookup( uchar* key ) const {
	int annInt = 0;
	if( getAnnotation( key, &annInt ) )
	    return annStrings_ + annInt;
	return 0;
    }


    inline void Cislex::loadFromFile( char* dicFile ) {
	FILE* fi = fopen( dicFile, "rb" );
	if ( !fi ) {
	    throw exceptions::badFileHandle( "csl::Cislex: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for reading." );
	}
	loadFromFile( fi );
	fclose( fi );
    }
    
    inline void Cislex::loadFromFile( FILE* fi ) {
	fread( &header_, sizeof( Header ), 1, fi );
	if ( ( header_.magicNumber_ != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "TransTable: Magic number comparison failed.\n" );
	}
	MinDic::loadFromFile( fi );
	annStrings_ = (uchar*) malloc( header_.sizeOfAnnStrings_ * sizeof( uchar ) );
	fread( annStrings_, sizeof( uchar ), header_.sizeOfAnnStrings_, fi );
	
    }

    inline void Cislex::writeToFile( char* dicFile ) const {
	FILE* fo = fopen( dicFile, "wb" );
	if ( !fo ) {
	    throw exceptions::badFileHandle( "Cislex: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for writing." );
	}
	writeToFile( fo );
	fclose( fo );
    }
    
    inline void Cislex::writeToFile( FILE* fo ) const {
	fwrite( &header_, sizeof( Header ), 1, fo );
	MinDic::writeToFile( fo );
	fwrite( annStrings_, sizeof( uchar ), header_.sizeOfAnnStrings_, fo );
    }


    inline void Cislex::initConstruction() {
	MinDic::initConstruction();
	
    }

    inline void Cislex::finishConstruction() {
	MinDic::finishConstruction();
    }

    inline void Cislex::compileDic( const char* lexFile ) {
	initConstruction();
	
	std::ifstream fileHandle( lexFile );
	if( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" + 
					     std::string( lexFile ) + 
					     "' for reading." );
	}
	uchar line[Global::lengthOfLongStr];
	uchar* key;
	uchar* annotationStr;

	while( fileHandle.getline(( char* ) line, Global::lengthOfLongStr ) )  {
	    /////////////////// PARSE THE INPUT STRING
	    uchar *c;
	    c = ( uchar* )strchr( ( char* )line, '.' );
	    
	    if( c ) {
		*c = 0;
		annotationStr = ( c + 1 );
	    }
	    key = line;

	    if( !c || !*annotationStr || !*key ) {
		throw exceptions::badInput( "Cislex: wrong input format" );
	    }

	    size_t offset = annHash_.findOrInsert( annotationStr );
	    
	    MinDic::addToken( key, offset );

	}
	fileHandle.close();

	finishConstruction();
    }

}


#endif
