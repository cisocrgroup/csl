#ifndef CSL_MINDIC_H
#define CSL_MINDIC_H CSL_MINDIC_H

#include <fstream>
#include <vector>
#include "../Global.h"
#include "../TransTable/TransTable.h"
#include "../Alphabet/Alphabet.h"

namespace csl {


    /**
     * class MinDic performs the construction and usage of minimal acyclic 
     * finite state automata for large finite dictionaries. 
     * It follows an algorithm described in 
     * Jan Daciuk et. al., Incremental Construction of Minimal Acyclic
     * Finite-State Automata, 2000.
     *
     * Many ideas for the implementation are adapted from a C-program written
     * by Stoyan Mihov.
     * 
     * @author Ulrich Reffle, <uli@reffle.de>
     * @date Apr 2006
     *
     */
    class MinDic : public TransTable< TOKDIC > {
    public:
	typedef TransTable< TOKDIC > TransTable_t;

	MinDic( const Alphabet& alph );

	void loadFromFile( char* binFile ) {
	    FILE* fi = fopen( binFile, "rb" );
	    loadFromStream( fi );
	    fclose( fi );
	}
	
	void loadFromStream( FILE* fi ) {
	    // read the MinDic-Header
	    fread( &header_, sizeof( Header ), 1, fi );

	    // read the TransTable
	    TransTable_t::loadFromFile( fi );
	    // read the annotations
	    annotations_ = new int[header_.nrOfKeys_];
	    fread( annotations_, sizeof( int ), header_.nrOfKeys_, fi );
	}
	
	void writeToFile( char* compFile ) const {
	    FILE* fo = fopen( compFile, "wb" );
	    writeToStream( fo );
	    fclose( fo );
	}

	void writeToStream( FILE* fo ) const {
	    // write the header
	    fwrite( &header_, sizeof( Header ), 1, fo );
	    // write the TransTable
	    TransTable_t::writeToFile( fo );
	    // write the annotations
	    fwrite( annotations_, sizeof( int ), header_.nrOfKeys_, fo );
	    
	}

	/**
	 * The funtion that actually executes the computation of the trie.
	 * @param txtFile The dictionary (including annotations) in txt format
	 */
	void compileDic( char* txtFile );


	void initConstruction();
	void finishConstruction();

	/**
	 * processes one input line: separates the key from the annotations (if present)
	 * and performs the insertion into the trie
loadFromFile	 * @arg a cstring pointing to the current line
	 */
	void addToken( const uchar* input, int value );

	/// extracts the trie to stdout
	void printDic( int initState ) const;

	inline uint_t walkStr( int state, const uchar* str ) const {
	    while( *str && state ) {
		state = walk( state, alph_.code( *str ) );
		++str;
	    }
	    return state;
	}

	
	/**
	 * This method from TransTable is blocked for MinDic, not implemented here!
	 */
	int getFirstAnn( uint_t state );

	inline bool getAnnotation( uchar* key, int* annotation ) const {
	    uint_t pos = getRoot();
	    size_t perfHashValue = 0;
	    while( *key && pos ) {
		pos = walkPerfHash( pos, alph_.code( *key ), perfHashValue );
		++key;
	    }
	    *annotation = annotationsAt( perfHashValue );
	    return( !*key && pos && isFinal( pos ) );
	}
	
	inline int getAnnotation( size_t perfHashValue ) const {
	    return annotationsAt( perfHashValue );
	}
	
	void printDic() const;

	size_t getNrOfKeys() const {
	    return header_.nrOfKeys_;
	}
	
    private:
#include "./StateHash.h"
	const Alphabet& alph_;

	mutable size_t count_; // is used for counting during printing

	// Those vars are used for construction
	TempState_t *tempStates_;
	StateHash* hashtable_;

//	uchar key_[Global::lengthOfStr];
	const uchar* key_;
	uchar lastKey_[Global::lengthOfLongStr];

	int* annotations_;
	size_t sizeOfAnnotationBuffer_;

	inline int annotationsAt( size_t n ) const {
	    if( n >= header_.nrOfKeys_ ) throw exceptions::outOfRange( "MinDic: out-of-range request for annotation-array." );
	    return annotations_[n];
	}

	inline int replaceOrRegister( TempState_t& state ) {
	    int storedState = 0;
	    if ( ( storedState = hashtable_->findState( state ) ) == 0 ) { // if equiv. state doesn't exist
		storedState = storeTempState( state ); // store it
		hashtable_->push( state, storedState ); // add it to the hashtable
	    }
	    return storedState;
	}
	void printDic_rec( int pos, int depth, size_t perfHashValue ) const;

	class Header {
	public:
	    static const bits64 magicNumber_ = 47247821;
	    size_t nrOfKeys_;
	    bool hasAnnotations_;
	};

	Header header_;

    };

} //eon

// #include "./MinDic.cxx"

#endif
