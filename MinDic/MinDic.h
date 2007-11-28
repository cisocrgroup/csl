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
    template< class AnnType = int >
    class MinDic : public TransTable< TOKDIC > {
    public:
	typedef TransTable< TOKDIC > TransTable_t;
	typedef AnnType AnnType_t;


	MinDic( const char* dicFile = 0 );

	inline void loadFromFile( const char* binFile );
	
	inline void loadFromStream( FILE* fi );
	
	inline void writeToFile( char* binFile ) const;

	inline void writeToStream( FILE* fo ) const;

	/**
	 * The funtion that actually executes the computation of the trie.
	 * @param txtFile The dictionary (including annotations) in txt format
	 */
	inline void compileDic( const char* txtFile );

	inline void parseAnnotation( wchar_t* str, AnnType_t* annotation ) const;

	inline void initConstruction();
	inline void finishConstruction();

	/**
	 * processes one input line: separates the key from the annotations (if present)
	 * and performs the insertion into the trie
	 * @arg a cstring pointing to the current line
	 */
	inline void addToken( const wchar_t* input, const AnnType_t& value );

	/// extracts the trie to stdout
	inline void printDic( StateId_t initState ) const;

	
	/**
	 * This method from TransTable is blocked for MinDic, not implemented here!
	 */
	inline int getFirstAnn( StateId_t state );

	/**
	 * Use this function to do a convenient lookup. If you don't need the annotation, just pass on
	 * a NULL-Pointer or omit the 2nd argument.
	 *
	 * @param[in] key a key of the dictionary
	 * @param[out] annotation a pointer where to store the found annotation. If NULL, then annotation is discarded
	 * @return true iff key was found in the dictionary
	 */
	inline bool lookup( const wchar_t* key, AnnType_t* annotation = 0 ) const;	
	/**
	 * @deprecacted this method is renamed to lookup()
	 */
	inline bool getAnnotation( const wchar_t* key, AnnType_t* annotation ) const;


	inline const AnnType_t& getAnnotation( size_t perfHashValue ) const;
	
	inline void printDic() const;

	inline size_t getNrOfKeys() const;

	inline void doAnalysis() const;
	
    protected:
	inline const AnnType_t& annotationsAt( size_t n ) const;

    private:
#include "./StateHash.h" // don't forget to include "StateHash.tcc" below
	
	mutable size_t count_; // is used for counting during printing

	// Those vars are used for construction
	TempState *tempStates_;
	StateHash* hashtable_;

	wchar_t lastKey_[Global::lengthOfLongStr];

	AnnType_t* annotations_;
	size_t sizeOfAnnotationBuffer_;

	size_t nrOfKeys_;

	inline StateId_t replaceOrRegister( TempState& state );

	void printDic_rec( StateId_t pos, int depth, size_t perfHashValue ) const;

	static const bits64 magicNumber_ = 47247821;
	class Header {
	public:
	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }
	    size_t getNrOfKeys() const {
		return nrOfKeys_;
	    }

	    void set( const MinDic& minDic ) {
		magicNumber_ = minDic.magicNumber_;
		nrOfKeys_ = minDic.nrOfKeys_;
	    }
	private:
	    bits64 magicNumber_;
	    bits64 nrOfKeys_;
	};

	Header header_;

    };


} //eon

#include "./MinDic.tcc"
#include "./StateHash.tcc"

#endif
