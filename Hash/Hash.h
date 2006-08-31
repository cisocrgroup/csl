#ifndef CSL_HASH_H
#define CSL_HASH_H CSL_HASH_H

#include<vector>

#include "../Global.h"
namespace csl {

    /**
     * Hash provides a hash table which stores and finds c-strings. The class is
     * designed to be used for hashing suso-strings for fast traversable dictionaries -
     *
     * Inserted strings are stored in a buffer here in Hash, the hash table stores pointers
     * into that buffer.
     *
     * @author Uli Reffle, <uli@reffle.de>
     */
    class Hash {
    public:
	/**
	 * constructor
	 *
	 * @param keyBuffer[in/out] a reference to a c-string, where the keys are stored. 
	 * @param estimatedNrOfKeys[in] an estimation that helps to choose the right size of the hashtable
	 * @param sizeOfeyBuffer[in/out]
	 * @todo think about the heuristics concerning the buffer initialization size
	 */
	inline Hash( size_t estimatedNrOfKeys, uchar*& keyBuffer, size_t& sizeOfKeyBuffer );
	
	/**
	 * insert a key into the hash table
	 * @param key
	 */
	inline size_t findOrInsert( const uchar* key );
	
	/**
	 * look up a key in the hash table
	 * @param key
	 * @return a value >0 iff the key was found, 0 otherwise
	 */
	inline size_t find(const uchar* key ) const;
	
	inline void reset();

	inline uint_t getTableSize() const;

	inline size_t getLengthOfKeyStrings() const;
    private:
	/**
	 * @return a const reference to the hashtable
	 */
	inline const std::vector< size_t >& getTable() const;

	/**
	 * computes a hashcode for a given string.
	 * The computation is based on the ascii-values so that no Alph-object
	 * is necessary.
	 * @return the hashcode for str
	 */
	inline uint_t getHashCode( const uchar* key ) const;

	inline const uchar* getStringAt( size_t offset ) const;

	inline uchar* getStringAt( size_t offset );

	inline void reallocKeyBuffer( size_t size );

	/**
	 * the data structure holding the actual table
	 */
	std::vector<uint_t> table_;

	/**
	 * counts the number of inserted keys
	 */
	uint_t nrOfKeys_;

	/**
	 * the buffer holding all the keys
	 * This is just a reference to the buffer handed over from the client
	 */
	uchar*& keyBuffer_;

	/**
	 * size of keyBuffer_
	 */
	size_t& sizeOfKeyBuffer_;

	/**
	 * offset just past the last terminating \0 of keyBuffer_
	 * equals: actual used size of the buffer
	 */
	size_t lengthOfKeyStrings_;

	/**
	 * We use closed hashing with linear probing. HASHC1 and HASHC2 are the two
	 * parameters determining the hash-function.
	 */
	/*@{*/
	static const int HASHC1 = 257;
	static const int HASHC2 = 107;
	/*@}*/
    };
    
    Hash::Hash( size_t estimatedNrOfKeys, uchar*& keyBuffer, size_t& sizeOfKeyBuffer ) : 
	table_( estimatedNrOfKeys * 15, 0 ),
	nrOfKeys_( 0 ),
	keyBuffer_( keyBuffer ),
	sizeOfKeyBuffer_( sizeOfKeyBuffer ),
	lengthOfKeyStrings_( 1 )
    {
	if( keyBuffer_ == 0 ) {
	    if( sizeOfKeyBuffer_ != 0 ) {
		printf( "Hash-Constructor: non-compatible buffer / size\n" );
		exit(1);
	    }
	    reallocKeyBuffer( estimatedNrOfKeys * 10 );
	}
    }
    
    size_t Hash::find( const uchar* key ) const {
	uint_t slot = getHashCode( key );
	while( table_.at( slot ) != 0 &&  // hash-slot non-empty
	       strcmp( (char*)key, (char*)getStringAt( table_.at( slot ) ) ) ) { // no match
	    slot = ( slot + HASHC2 ) % getTableSize();
	}
	return table_.at( slot );
    }

    size_t Hash::findOrInsert( const uchar* key ) {
	uint_t slot = getHashCode( key );
	while( table_.at( slot ) != 0 &&  // hash-slot non-empty
	       strcmp( (char*)key, (char*)getStringAt( table_.at( slot ) ) ) ) { // no match
	    slot = ( slot + HASHC2 ) % getTableSize();
	}

	if( table_.at( slot ) == 0 ) { // key wasn't found
	    size_t lengthOfKey = strlen( (char*)key );
	    // resize keyBuffer_ if necessary
	    while( ( lengthOfKeyStrings_ + lengthOfKey) > sizeOfKeyBuffer_ ) {
		reallocKeyBuffer( (int) (sizeOfKeyBuffer_ * 1.5) );
	    }
	    strncpy( (char*)(keyBuffer_ + lengthOfKeyStrings_), (char*)key, lengthOfKey );
	    table_.at( slot ) = lengthOfKeyStrings_;
	    lengthOfKeyStrings_ += lengthOfKey + 1;
//	    printf( "Create: %s at slot %d, buf_offset %d\n", key, slot, table_.at( slot ) );
	}
	else {
//	    printf( "Found: %s at slot %d, buf_offset %d\n", key, slot, table_.at( slot ) );
	}

	return table_.at( slot );
    }
    
    inline const uchar* Hash::getStringAt( size_t offset ) const {
	return ( keyBuffer_ + offset );
    }

    inline uchar* Hash::getStringAt( size_t offset ) {
	return ( keyBuffer_ + offset );
    }

    inline void Hash::reset() {
	lengthOfKeyStrings_ = 0;
	nrOfKeys_ = 0;
	memset( &table_[0], 0, table_.size() * sizeof(uint_t) );
    }

    uint_t Hash::getTableSize() const {
	return table_.size();
    }

    size_t Hash::getLengthOfKeyStrings() const {
	return lengthOfKeyStrings_;
    }

    uint_t Hash::getHashCode( const uchar* str ) const {
	uint_t h = 0;
	while(*str) {
	    h = h * 256 + *str;
	    ++str;
	}
	return ( h % getTableSize() );
    }

    const std::vector<uint_t>& Hash::getTable() const {
	return table_;
    }

    inline void Hash::reallocKeyBuffer( size_t newSize ) {
	keyBuffer_ = (uchar*) realloc( keyBuffer_, newSize );
	sizeOfKeyBuffer_ = newSize;
	if( keyBuffer_ == 0 ) {
	    std::cerr<<"Hash realloc: Out of memory. Couldn't resize to "<<newSize<<" bytes\n"<<std::endl;
	    exit( 1 );
	}
    }


}

#endif
