#ifndef CSL_POS_PATTERN
#define CSL_POS_PATTERN CSL_POS_PATTERN

#include "./Pattern.h"


namespace csl {

    /**
     * @brief the Pattern class is simply enriched by a member variable indicating a position inside a word
     */
    class PosPattern : public PatternSet::PatternRef {
    public:
	/**
	 * @brief creates an empty Pattern at position 0
	 */
	PosPattern();

	PosPattern( const PatternRef& patternRef, size_t position );
	
	// PosPattern( const std::wstring& left, const std::wstring& right, size_t position );
	
	size_t getPosition() const;

	void print( std::wostream& os = std::wcout ) const;
	
    private:
	size_t position_;
	
    }; // class PosPattern
    
    PosPattern::PosPattern() :
	PatternRef(),
	position_( 0 ) {
    }

    PosPattern::PosPattern( const PatternRef& patternRef, size_t position ) :
	PatternRef( patternRef ),
	position_( position ) {
    }
    
//     PosPattern::PosPattern( const std::wstring& left, const std::wstring& right, size_t position ) :
// 	Pattern( left, right ),
// 	position_( position ) {
//     }

    size_t PosPattern::getPosition() const {
	return position_;
    }

    void PosPattern::print( std::wostream& os ) const {
	os << "(";
	PatternRef::print( os );
	os << "," << getPosition() << ")";

    }


} // eon


#endif
