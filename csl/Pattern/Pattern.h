#ifndef CSL_PATTERN_H
#define CSL_PATTERN_H CSL_PATTERN_H

#include<string>
#include<iostream>

namespace csl {

    /**
     * @brief Pattern holds one string substitution alpha --> beta which is allowed in a certain context.
     *
     * The Pattern object simply has two string components left and right. If both are empty, the pattern is
     * something like a "neutral element", or a dummy. Then, empty() returns true. It is sometimes needed where
     * the implementation expects a Pattern without there really being one.
     *
     * @author Ulrich Reffle, 2008
     */
    class Pattern {
    public:
	/**
	 * @brief Constructs an empty pattern with left == right == ""
	 */
	inline Pattern();

	/**
	 * @brief A very self-explaining constructor.
	 */
	inline Pattern( const std::wstring& left, const std::wstring& right );

	inline bool operator== ( const Pattern& other ) const {
	    return ( ( getLeft() == other.getLeft() ) &&
		     ( getRight() == other.getRight() ) );
	}

	inline bool operator< ( const Pattern& other ) const {
	    if( getLeft() < other.getLeft() ) {
		return true;
	    }
	    else if( getLeft() > other.getLeft() ) {
		return false;
	    }
	    else return ( getRight() < other.getRight() );
	}

	inline void clear() {
	    left_.clear();
	    right_.clear();
	}

	/**
	 * @brief get the left side of the pattern
	 * @return the left side of the pattern
	 */
	inline const std::wstring& getLeft() const;

	/**
	 * @brief get the right side of the pattern
	 * @return the right side of the pattern
	 */
	inline const std::wstring& getRight() const;
	
	/**
	 * @brief returns if pattern is "empty"
	 */
	inline bool empty() const {
	    return ( left_.empty() && right_.empty() );
	}

	inline void setLeft( std::wstring const& left );

	inline void setRight( std::wstring const& right );

	inline void print( std::wostream& os = std::wcout ) const;

	inline std::wstring toString() const;
	

    private:
	std::wstring left_;
	std::wstring right_;
    };

    Pattern::Pattern() {
    }

    Pattern::Pattern( const std::wstring& left, const std::wstring& right ) :
	left_( left ),
	right_( right ) {
    }
    
    const std::wstring& Pattern::getLeft() const {
	return left_;
    }
    
    const std::wstring& Pattern::getRight() const {
	return right_;
    }
    
    inline void Pattern::setLeft( std::wstring const& left ) {
	left_ = left;
    }

    inline void Pattern::setRight( std::wstring const& right ) {
	right_ = right;
    }

    void Pattern::print( std::wostream& os ) const {
	os << getLeft() << "_" << getRight();
    }

    std::wstring Pattern::toString() const {
	return getLeft() + L"_" + getRight();
    }
    


} // eon


#endif
