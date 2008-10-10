#ifndef CSL_FBDIC_H
#define CSL_FBDIC_H CSL_FBDIC_H

#include<vector>
#include<string>
#include<algorithm>
#include "../Global.h"
#include "../MinDic/MinDic.h"

namespace csl {

    /**
     * Class FBDic ("Foward-Backward-Dictionary") covers a dictionary and its reversed variant in one 
     * object and also, more importantly, in one binary file on the hard disk.
     *
     * @author: Uli Reffle
     */
    template< class AnnType = int >
    class FBDic {
    public:
	typedef AnnType AnnType_t;
	typedef MinDic< AnnType_t > MinDic_t;


	FBDic( const char* dicFile = 0 );
    
	/**
	 * returns a reference to the forward dictionary
	 * @returns a reference to the forward dictionary
	 */
	const MinDic_t& getFWDic() const;

	/**
	 * returns a reference to the backward dictionary
	 * @returns a reference to the backward dictionary
	 */
	const MinDic_t& getBWDic() const;

	/**
	 * compiles an FBDic from a .lex file. Consult the @link fbDic_manual FDic Manual@endlink
	 * for details on the format of the input file.
	 */
	void compileDic( const char* txtFile );

	/**
	 * @name Loading from /writing to hard disk
	 */
	//@{
	/**
	 * @brief Load a compiled FBDic from hard disk.
	 * @param a file containing a compiled FBDic. (often: *.fbdic)
	 */
	inline void loadFromFile( const char* binFile );
	
	/**
	 * @brief Load a compiled FBDic from an open file stream.
	 * @param fi a c-style file pointer.
	 */
	inline void loadFromStream( FILE* fi );
	
	/**
	 * @brief dump FBDic automaton to a file in binary form.
	 * @param binFile File to write the automaton into.
	 */
	inline void writeToFile( char* binFile ) const;

	/**
	 * @brief dump FBDic automaton to an open file stream
	 * @param fo a c-style file pointer.
	 */
	inline void writeToStream( FILE* fo ) const;

	// @}

    private:
	MinDic_t fwDic_;
	MinDic_t bwDic_;

	/**
	 * During construction, used to represent one entry of the dictionary
	 */
	class Entry {
	public:
	    Entry( const std::wstring& key, const AnnType_t& annotation ) :
		key_( key ),
		annotation_( annotation ) {
	    }
	    const std::wstring& getKey() const {
		return key_;
	    }
	    const AnnType_t& getAnnotation() const {
		return annotation_;
	    }

	    bool operator<( const Entry& other ) const {
		return this->getKey() < other.getKey();
	    }

	private:
	    std::wstring key_;
	    AnnType_t annotation_;
	};

	std::vector< Entry > entries_;

	static const bits64 magicNumber_ = 345783116;
	class Header {
	public:
	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }

	    void set( const FBDic& fbDic ) {
		magicNumber_ = fbDic.magicNumber_;
	    }
	private:
	    bits64 magicNumber_;
	};

	Header header_;

    };

} // eon

#include "./FBDic.tcc"

#endif
