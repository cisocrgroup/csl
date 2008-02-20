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
	
    private:

    }; // class Instruction


} // eon


#endif
