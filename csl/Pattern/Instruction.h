#ifndef CSL_INSTRUCTION
#define CSL_INSTRUCTION CSL_INSTRUCTION

#include<fstream>
#include "./PosPattern.h"


namespace csl {

    /**
     * @brief Instruction is a std::vector< PosPattern>, and it's an instruction in the sense we use the word in connection with spelling variants.
     */
    class Instruction : public std::vector< PosPattern > {
    public:

	/**
	 * @brief prints a string-representation of the instruction to stdout or to another std::wstream 
	 * specified as argument.
	 */
	void print(  std::wostream& os = std::wcout ) const {
	    os<<"[";
	    for( const_iterator it = begin(); it != end(); ++it  ) {
		it->print( os );
	    }
	    os<<"]";
	}
	
	/**
	 * @brief apply the instruction to the string passed as pointer argument
	 * @param[in/out] str the string that is to be changed by the instruction
	 *
	 * Note that at the moment the method does not check if the instruction is applicable to @c str,
	 * i.e. it does not check if the left sides of the patterns really occur in @c str at the given
	 * positions. If the instruction specifies a transformation beyond the word border, most likely
	 * an exception will be thrown.
	 *
	 * @todo make this method more secure, at least handle the exception that are maybe thrown by std::wstring::replace
	 */
	void applyTo( std::wstring* str ) const {
	    // iterate through PosPatterns in reverse order: that way the positions remain intact
	    // even if the Patterns change the word length
	    for( const_reverse_iterator rit = rbegin(); rit != rend(); ++rit ) {
		// std::wcout<<"pos="<< rit->getPosition()<<"left="<<rit->getLeft()<<",llength="<< rit->getLeft().length()<<", right="<< rit->getRight()<<std::endl; // DEBUG
		str->replace( rit->getPosition(), rit->getLeft().length(), rit->getRight() );
	    }
	}

    private:

    }; // class Instruction


} // eon

namespace std {
    inline wostream& operator<<( wostream& os, csl::Instruction const& obj ) {
	obj.print( os );
	return os;
    }
}


#endif
