#ifndef CSL_VARIANT_RECOGNIZER_H
#define CSL_VARIANT_RECOGNIZER_H CSL_VARIANT_RECOGNIZER_H

#include "../Pattern/Instruction.h"

namespace csl {

    /**
     * @brief This is an interface for classes which have the following ability:
     * For a query word they decide if this word can be interpreted as a variant
     * of another word from a given dictionary. 
     *
     * A variant in this context is a word of the background dictionary that is modified
     * by the application of a number of string replace patterns.
     *
     * @author Ulrich Reffle, 2008
     */
    class VariantRecognizer {
    public:
	class Answer {
	public:
	    std::wstring const& getWord() const {
		return word;
	    }

	    std::wstring const& getBaseWord() const {
		return baseWord;
	    }

	    Instruction const& getInstruction() const {
		return instruction;
	    }

	    /// the suggested correction candidate
	    std::wstring word;
	    /// the underlying word in the modern dictionary
	    std::wstring baseWord;
	    /// the coresponding Instruction: word = baseWord + instruction
	    Instruction instruction;

	    size_t levDistance;

	    void print( std::wostream& os = std::wcout ) const {
		os<<word<<":"<<baseWord<<"+";
		instruction.print( os );
		os<<",dist="<<levDistance;
	    }
	    
	}; // class Answer

	virtual bool query( const std::wstring& word, std::vector< Answer >* answers ) const = 0; 

	/**
	 * @brief For instaces of a VariantRecognizer which returns only one interpretation
	 *
	 * Also methods providing various interpretations may choose to return one of them
	 * (maybe the one with the highest score of some sort).
	 *
	 */
	virtual bool query( const std::wstring& word, Answer* answer ) const = 0;

    };


}


#endif
