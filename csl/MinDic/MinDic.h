#ifndef CSL_MINDIC_H
#define CSL_MINDIC_H CSL_MINDIC_H

#include <fstream>
#include <vector>
#include <errno.h>
#include "../Global.h"
#include "../codecvt/UTF8_Codecvt.h"
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
     * Note that a MinDic inherits from TransTable and so offers all of TransTable's 
     * interface to the guts of the automaton. However this might change in the future.
     *
     * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
     * @date Apr 2006
     *
     */
    template< class AnnType = int >
    class MinDic : public TransTable< TOKDIC > {
    public:
	typedef TransTable< TOKDIC > TransTable_t;
	/**
	 * @brief The datatype of the annotation that comes with each dictionary entry.
	 * Note that \c AnnType_t, currently,  has to be a trivial datatype: if you store
	 * an object x of type AnnType_t, the MinDic will store exactly sizeof(AnnType_t) bytes,
	 * beginning at address &x. Heap data will be lost.
	 */
	typedef AnnType AnnType_t;

	/**
	 * This class provides a much easier interface to the MinDic if the guts of the automaton
	 * have to be explored state- and transition-wise.
	 */
	class State {
	public:
	    /**
	     * @param a reference to the \c MinDic the \c State belongs to.
	     */
	    State( const MinDic< AnnType >& minDic ) :
		minDic_( &minDic ),
		dicPos_( minDic_->getRoot() ),
		perfHashValue_( 0 ) {
	    }
	    
	    /**
	     * 
	     */
	    bool walk( wchar_t c ) {
		dicPos_ = minDic_->walkPerfHash( dicPos_, c, perfHashValue_ );
		return isValid();
	    }

	    bool isValid() {
		return ( dicPos_ != 0 );
	    }

	    bool hasTransition( wchar_t c ) const {
		return minDic_->walk( dicPos_, c );
	    }

	    State getTransTarget( wchar_t c ) const {
		size_t tmpPHValue = perfHashValue_;
		StateId_t newPos = minDic_->walkPerfHash( dicPos_, c, tmpPHValue );
		return State( *minDic_, newPos, tmpPHValue );
	    }

	    const wchar_t* getSusoString() const {
		return minDic_->getSusoString( dicPos_ );
	    }

	    size_t getPerfHashValue() const {
		return perfHashValue_;
	    }

	    StateId_t getStateID() const {
		return dicPos_;
	    }

	    bool isFinal() const {
		return minDic_->isFinal( dicPos_ );
	    }
	    
	    const AnnType& getAnnotation() {
		return minDic_->getAnnotation( getPerfHashValue() );
	    }

	private:

	    State( const MinDic< AnnType >& minDic, StateId_t dicPos, size_t perfHashValue ) :
		minDic_( &minDic ),
		dicPos_( dicPos ),
		perfHashValue_( perfHashValue ) {
	    }

	    const MinDic< AnnType >* minDic_;
	    StateId_t dicPos_;
	    size_t perfHashValue_;
	}; // class State

	/**
	 * @brief Create a new MinDic. An optional file-path as argument invokes a call to loadFromFile.
	 * @param a file containing a compiled MinDic. (optional; often: *.mdic)
	 */
	MinDic( const char* dicFile = 0 );


	/**
	 * @name Lookup
	 */
	//@{

	/**
	 * Use this function to do a convenient lookup. If you don't need the annotation, just pass on
	 * a NULL-Pointer or omit the 2nd argument.
	 *
	 * @param[in] key a key of the dictionary
	 * @param[out] annotation a pointer where to store the found annotation. If NULL, then annotation is discarded
	 * @return true iff key was found in the dictionary
	 */
	inline bool lookup( const wchar_t* key, AnnType_t* annotation = 0 ) const;

	inline bool lookup( std::wstring const& key, AnnType_t* annotation = 0 ) const;

	/**
	 * @deprecacted this method is renamed to lookup()
	 */
	inline bool getAnnotation( const wchar_t* key, AnnType_t* annotation ) const;


	inline const AnnType_t& getAnnotation( size_t perfHashValue ) const;

	/**
	 * Get a State object of the automaton's root/ start state. 
	 * @return a State object of the automaton's root/ start state. 
	 * @see State
	 */
	inline State getRootState() const {
	    return State( *this );
	}

	//@}


	/**
	 * @name Loading from /writing to hard disk
	 */
	//@{
	/**
	 * @brief Load a compiled MinDic from hard disk.
	 * @param a file containing a compiled MinDic. (often: *.mdic)
	 */
	inline void loadFromFile( const char* binFile );
	
	/**
	 * @brief Load a compiled MinDic from an open file stream.
	 * @param fi a c-style file pointer.
	 */
	inline void loadFromStream( FILE* fi );
	
	/**
	 * @brief dump MinDic automaton to a file in binary form.
	 * @param binFile File to write the automaton into.
	 */
	inline void writeToFile( char const* binFile ) const;

	/**
	 * @brief dump MinDic automaton to an open file stream
	 * @param fo a c-style file pointer.
	 */
	inline void writeToStream( FILE* fo ) const;

	//@}
	
	/**
	 * @name Construction of a new MinDic
	 */
	//@{
	/**
	 * @brief The funtion that actually executes the computation of the trie.
	 * @todo Should this method be public??? Maybe not ...
	 * @param txtFile The dictionary (including annotations) in txt format
	 */
	inline void compileDic( const char* txtFile );

	/**
	 * @todo add documentation
	 */
	inline void parseAnnotation( std::wstring* str, AnnType_t* annotation ) const;

	/**
	 * @brief prepares the object for construction of a new MinDic.
	 * Call before calling addToken() for the first time. 
	 */
	inline void initConstruction();

	inline void finishConstruction();

	/**
	 * processes one input line: separates the key from the annotations (if present)
	 * and performs the insertion into the trie
	 * @arg a cstring pointing to the current line
	 */
	inline void addToken( const wchar_t* input, const AnnType_t& value );

	//@}

	/**
	 * @name Convenience, Information, Debug
	 */
	//@{

	inline void printDic() const;

	/// extracts the trie to stdout
	inline void printDic( StateId_t initState ) const;

	inline size_t getNrOfKeys() const;

	inline void doAnalysis() const;

	/**
	 * This method from TransTable is blocked for MinDic, not implemented here!
	 */
	inline int getFirstAnn( StateId_t state );

 	//@}

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
		return (size_t)nrOfKeys_;
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
