#ifndef CSL_POS_PATTERN
#define CSL_POS_PATTERN CSL_POS_PATTERN

#include<sstream>
#include "./Pattern.h"


namespace csl {

    /**
     * @brief the Pattern class is simply enriched by a member variable indicating a position inside a word
     */
    class PosPattern : public Pattern {
    public:
	/**
	 * @brief creates an empty Pattern at position 0
	 */
	inline PosPattern();
	
	inline PosPattern( const std::wstring& left, const std::wstring& right, size_t position );
	
	inline bool operator==( PosPattern const& other ) const;

	inline size_t getPosition() const;
	
	inline void print( std::wostream& os = std::wcout ) const;
	inline std::wstring toString() const;
	
    private:
	size_t position_;
    }; // class PosPattern
    
    PosPattern::PosPattern() :
	Pattern(),
	position_( 0 ) {
    }

    PosPattern::PosPattern( const std::wstring& left, const std::wstring& right, size_t position ) :
 	Pattern( left, right ),
 	position_( position ) {
    }

    bool PosPattern::operator==( PosPattern const& other ) const {
	return ( Pattern::operator==( other ) && ( getPosition() == other.getPosition() ) );
    }

    
    size_t PosPattern::getPosition() const {
	return position_;
    }

    void PosPattern::print( std::wostream& os ) const {
	os << "(";
	Pattern::print( os );
	os << "," << getPosition() << ")";

    }

    std::wstring PosPattern::toString() const {
	//return std::wstring( L"(" ) + Pattern::toString() + L"," + swprintf( L"%ls", getPosition() ) + L")";
	std::wostringstream oss;
	print( oss );
	oss.flush();
	return oss.str();
    }


} // eon

namespace std {
    inline wostream& operator<<( wostream& os, csl::PosPattern const& obj ) {
	obj.print( os );
	return os;
    }
}

#endif
