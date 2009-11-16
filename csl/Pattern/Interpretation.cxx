#ifndef CSL_INTERPRETATION_CXX
#define CSL_INTERPRETATION_CXX CSL_INTERPRETATION_CXX

#include "./Interpretation.h"

namespace csl {

    Interpretation::Interpretation() :
	levDistance_( 0 ),
	baseWordScore_( -1 )
    {
    }

    bool Interpretation::operator<( Interpretation const& other ) const {
	float compare = 
	    ( getInstruction().size() + getLevDistance() * 1.01 ) - 
	    ( other.getInstruction().size() + other.getLevDistance() * 1.01 );
	    
	if     ( compare < 0 ) return true;
	else if( compare > 0 ) return false;

	compare = wcscmp( getWord().c_str(), other.getWord().c_str() );
	if( compare != 0 ) return ( compare >= 0 );
	    
	// otherwise: do it the very un-efficient way:
	return ( wcscmp( toString().c_str(), other.toString().c_str() ) > 0 );
    }

    std::wstring const& Interpretation::getWord() const {
	return word_;
    }

    std::wstring const& Interpretation::getBaseWord() const {
	return baseWord_;
    }

    size_t Interpretation::getLevDistance() const {
	return levDistance_;
    }

    size_t Interpretation::getBaseWordScore() const {
	return baseWordScore_;
    }

    Instruction& Interpretation::getInstruction() {
	return instruction_;
    }

    Instruction const& Interpretation::getInstruction() const {
	return instruction_;
    }

    Instruction& Interpretation::getHistInstruction() {
	return instruction_;
    }

    Instruction const& Interpretation::getHistInstruction() const {
	return instruction_;
    }

    void Interpretation::setWord( std::wstring const& w ) {
	word_ = w;
    }

    void Interpretation::setBaseWord( std::wstring const& w ) {
	baseWord_ = w;
    }

    void Interpretation::setLevDistance( size_t dist ) {
	levDistance_ = dist;
    }

    void Interpretation::setBaseWordScore( size_t score ) {
	baseWordScore_ = score;
    }

    void Interpretation::clear() {
	word_.clear();
	baseWord_.clear();
	levDistance_ = 0;
	baseWordScore_ = -1;
	instruction_.clear();
    }

    size_t Interpretation::parseFromString( std::wstring const& str, size_t offset ) {
	size_t colon = str.find( ':', offset );
	if( colon == str.npos ) throw exceptions::badInput( "csl::Interpretation::parseFromString: Found no colon" );
	setWord( str.substr( offset, colon - offset ) );

	offset = colon + 1;
	size_t plus = str.find( '+', offset );
	if( plus == str.npos ) throw exceptions::badInput( "csl::Interpretation::parseFromString: Found no plus" );
	setBaseWord( str.substr( offset, plus - colon - 1 ) );

	offset = plus + 1;

	offset = instruction_.parseFromString( str, offset );

	if( str.substr( offset, 6 ) != L",dist=" ) throw exceptions::badInput( "csl::Interpretation::parseFromString: Found no ',dist='" );
	offset += 6;
	size_t end = str.find_first_not_of( L"0123456789", offset );
	if( end == str.npos ) end = str.size();
	wchar_t* dummy = 0;
	levDistance_ = wcstol( str.substr( offset, end - offset ).c_str(), &dummy, 10 );

	baseWordScore_ = -1;
	
	return end;

    }

    void Interpretation::print( std::wostream& os ) const {
	os<<word_<<":"<<baseWord_<<"+";
	instruction_.print( os );
	os<<",dist="<<levDistance_;
	//os<<",baseWordScore="<<baseWordScore_;
    }
	
    std::wstring Interpretation::toString() const {
	//return word_ + L":" + baseWord_ + L"+" + instruction_.toString() + L",dist=" + levDistance_;
	std::wostringstream oss;
	print( oss );
	oss.flush();
	return oss.str();
    }

    void Interpretation::print_v2( std::wostream& os ) const {
	size_t begin = 0;
	for( csl::Instruction::const_iterator posPattern = instruction_.begin(); posPattern != instruction_.end(); ++posPattern ) {
	    os<<baseWord_.substr( begin, posPattern->getPosition() - begin ); // print unchanged substring left of the pattern
	    os<<"["<<posPattern->getLeft()<<"->"<<posPattern->getRight()<<"]"; // print pattern
	    begin = posPattern->getPosition() + posPattern->getLeft().length();
	}
	os<<baseWord_.substr( begin ); // print unchanged suffix of the baseWord
	    
    }

} // eon

#endif