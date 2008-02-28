#ifndef CSL_INSTRUCTION
#define CSL_INSTRUCTION CSL_INSTRUCTION

#include<fstream>
#include "./PosPattern.h"


namespace csl {

    /**
     * @brief Represents an instruction in the sense we use the word in connection with orthogrpahical variants.
     */
    class Instruction : public std::vector< PosPattern > {
    public:
	void print(  std::wostream& os = std::wcout ) const {
	    std::wcout<<"[";
	    for( const_iterator it = begin(); it != end(); ++it  ) {
		it->print();
	    }
	    std::wcout<<"]";
	}
	
	void applyTo( std::wstring* str ) const {
	    // iterate through PosPatterns in reverse order: that way the positions remain intact
	    // even if the Patterns change the word length
	    for( const_reverse_iterator rit = rbegin(); rit != rend(); ++rit ) {
		str->replace( rit->getPosition(), rit->getLeft().length(), rit->getRight() );
	    }
	}

    private:

    }; // class Instruction


} // eon


#endif
