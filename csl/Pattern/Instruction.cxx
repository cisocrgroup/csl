#ifndef CSL_INSTRUCTION_CXX
#define CSL_INSTRUCTION_CXX CSL_INSTRUCTION_CXX

#include "./Instruction.h"
namespace csl {

    void Instruction::print(  std::wostream& os ) const {
	os<<"[";
	for( const_iterator it = begin(); it != end(); ++it  ) {
	    it->print( os );
	}
	os<<"]";
    }
	
    std::wstring Instruction::toString() const {
	std::wstring ret = L"[";
	for( const_iterator it = begin(); it != end(); ++it  ) {
	    ret += it->toString();
	}
	ret += L"]";
	
	return ret;
    }

    size_t Instruction::parseFromString( std::wstring const& str, size_t offset ) {
	if( ! this->empty() ) throw exceptions::badInput( "csl::Instruction::parseFromString: Instruction not empty" );
	
	if( str.at( offset ) != '[' ) throw exceptions::badInput( "csl::Instruction::parseFromString: Found no opening square bracket" );
	offset += 1;
	
	csl::PosPattern pp;
	while( str.at( offset ) != ']' ) {
	    pp.clear();
	    offset = pp.parseFromString( str, offset );
	    push_back( pp );
	}
	if( str.at( offset ) != ']' ) throw exceptions::badInput( "csl::Instruction::parseFromString: Found no closing square bracket" );
	offset += 1;

	return offset;
    }

    void Instruction::applyTo( std::wstring* str ) const {
	// iterate through PosPatterns in reverse order: that way the positions remain intact
	// even if the Patterns change the word length
	for( const_reverse_iterator rit = rbegin(); rit != rend(); ++rit ) {
	    // std::wcout<<"pos="<< rit->getPosition()<<"left="<<rit->getLeft()<<",llength="<< rit->getLeft().length()<<", right="<< rit->getRight()<<std::endl; // DEBUG
	    str->replace( rit->getPosition(), rit->getLeft().length(), rit->getRight() );
	}
    }
    

} // eon

#endif
