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

	// @{
	// @name Constructors/ Destructors

	Interpretation() :
	    baseWordScore_( -1 ),
	    levDistance_( 0 )
	    {
	    }

	// @} // END Constructors/ Destructors

	// @{
	// @name Getters

	/**
	 * @brief returns the variant/error string
	 */
	std::wstring const& getWord() const {
	    return word_;
	}

	/**
	 * @brief returns the baseWord, usually a word from a dictionary that was changed into a variant
	 */
	std::wstring const& getBaseWord() const {
	    return baseWord_;
	}

	size_t getLevDistance() const {
	    return levDistance_;
	}

	size_t getBaseWordScore() const {
	    return baseWordScore_;
	}

	/**
	 * @brief returns the instruction that was used to turn @c baseWord into @c word 
	 */
	Instruction& getInstruction() {
	    return instruction_;
	}

	/**
	 * @brief const version of getInstruction()
	 */
	Instruction const& getInstruction() const {
	    return instruction_;
	}

	// @} // END Getters

	// @{
	// @name Setters

	void setWord( std::wstring const& w ) {
	    word_ = w;
	}

	void setBaseWord( std::wstring const& w ) {
	    baseWord_ = w;
	}

	void setLevDistance( size_t dist ) {
	    levDistance_ = dist;
	}

	void setBaseWordScore( size_t score ) {
	    baseWordScore_ = score;
	}

	/**
	 * @brief erase all values from the object and bring it into a state as if just created.
	 */
	void clear() {
	    word_.clear();
	    baseWord_.clear();
	    levDistance_ = 0;
	    baseWordScore_ = -1;
	    instruction_.clear();
	}
	// @} // END Setters


	// @{
	// @name Pretty-Print
	/**
	 * @brief prints a string-representation of the interpretation to stdout or to another std::wstream 
	 * specified as argument.
	 */
	void print( std::wostream& os = std::wcout ) const {
	    os<<word_<<":"<<baseWord_<<"+";
	    instruction_.print( os );
	    os<<",dist="<<levDistance_;
	    os<<",baseWordScore="<<baseWordScore_;
	}

	/**
	 * @brief prints another useful format of the interpretation, more in the INL tradition
	 */
	void print_v2( std::wostream& os = std::wcout ) const {
	    size_t begin = 0;
	    for( csl::Instruction::const_iterator posPattern = instruction_.begin(); posPattern != instruction_.end(); ++posPattern ) {
		os<<baseWord_.substr( begin, posPattern->getPosition() - begin ); // print unchanged substring left of the pattern
		os<<"["<<posPattern->getLeft()<<"->"<<posPattern->getRight()<<"]"; // print pattern
		begin = posPattern->getPosition() + posPattern->getLeft().length();
	    }
	    os<<baseWord_.substr( begin ); // print unchanged suffix of the baseWord
	    
	}

	// @} // END Pretty-Print

    private:
	/// the suggested correction candidate
	std::wstring word_;

	/// the underlying word in the modern dictionary
	std::wstring baseWord_;

	/// the corresponding Instruction: word = baseWord + instruction
	Instruction instruction_;

	/**
	 * @brief 
	 *
	 */
	size_t levDistance_;

	/**
	 * @brief A score for the baseWord as was annotated in the dictionary or some other resource
	 *
	 * Most likely this would be a frequency score.
	 */
	int baseWordScore_;


	    
    }; // class Interpretation

}

#endif
