#ifndef CSL_INTERPRETATION_H
#define CSL_INTERPRETATION_H CSL_INTERPRETATION_H

#include "./Instruction.h"

namespace csl {
    /**
     * @brief This class is something like a container class to store one interpretation of some
     * string as the spelling variant (and/or erroneous variant) of some other string.
     *
     * Usually this class will be the answer object (or part thereof) of a query to ErrDic or Vaam.
     *
     * @author Ulrich Reffle 2008
     */
    class Interpretation {
    public:
	enum Status { undef, modern, approved_historic, hypothetical };

	Interpretation() :
	    baseWordScore( -1 ),
	    levDistance( 0 ),
	    status_( undef )
	    {
	    }

	/**
	 * @brief erase all values from the object and bring it into a state as if just created.
	 */
	void clear() {
	    baseWordScore = -1;
	    levDistance = 0;
	    word.clear();
	    baseWord_.clear();
	    instruction.clear();
	}

	/**
	 * @brief returns the variant/error string
	 */
	std::wstring const& getWord() const {
	    return word;
	}

	void setBaseWord( std::wstring const& w ) {
	    baseWord_ = w;
	}

	/**
	 * @brief returns the baseWord, usually a word from a dictionary that was changed into a variant
	 */
	std::wstring const& getBaseWord() const {
	    return baseWord_;
	}

	/**
	 * @brief returns the instruction that was used to turn @c baseWord into @c word 
	 */
	Instruction getInstruction() {
	    return instruction;
	}

	/**
	 * @brief const version of getInstruction
	 */
	Instruction const& getInstruction() const {
	    return instruction;
	}

	void setStatus( Status const& st ) {
	    status_ = st;
	}

	Status getStatus() const {
	    return status_;
	}

	/// the suggested correction candidate
	std::wstring word;


	/// the corresponding Instruction: word = baseWord + instruction
	Instruction instruction;

	/**
	 * @brief A score for the baseWord as was annotated in the dictionary or some other resource
	 *
	 * Most likely this would be a frequency score.
	 */
	int baseWordScore;

	/**
	 * @brief 
	 *
	 */
	size_t levDistance;

	/**
	 * @brief prints a string-representation of the interpretation to stdout or to another std::wstream 
	 * specified as argument.
	 */
	void print( std::wostream& os = std::wcout ) const {
	    os<<word<<":"<<baseWord_<<"+";
	    instruction.print( os );
	    os<<",dist="<<levDistance;
	    os<<",baseWordScore="<<baseWordScore;
	}

	/**
	 * @brief prints another useful format of the interpretation, more in the INL tradition
	 */
	void print_v2( std::wostream& os = std::wcout ) const {
	    size_t begin = 0;
	    for( csl::Instruction::const_iterator posPattern = instruction.begin(); posPattern != instruction.end(); ++posPattern ) {
		os<<baseWord_.substr( begin, posPattern->getPosition() - begin ); // print unchanged substring left of the pattern
		os<<"["<<posPattern->getLeft()<<"->"<<posPattern->getRight()<<"]"; // print pattern
		begin = posPattern->getPosition() + posPattern->getLeft().length();
	    }
	    os<<baseWord_.substr( begin ); // print unchanged suffix of the baseWord
	    
	}
    private:
	Status status_;

	/// the underlying word in the modern dictionary
	std::wstring baseWord_;
	    
    }; // class Interpretation

}

#endif
