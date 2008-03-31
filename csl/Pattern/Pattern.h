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
	Pattern();

	/**
	 * @brief A very self-explaining constructor.
	 */
	Pattern( const std::wstring& left, const std::wstring& right );

	/**
	 * @brief get the left side of the pattern
	 * @return the left side of the pattern
	 */
	const std::wstring& getLeft() const;

	/**
	 * @brief get the right side of the pattern
	 * @return the right side of the pattern
	 */
	const std::wstring& getRight() const;

	/**
	 * @brief returns if pattern is "empty"
	 */
	bool empty() const {
	    return ( left_.empty() && right_.empty() );
	}

	void print( std::wostream& os = std::wcout ) const;
	
	

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
    
    void Pattern::print( std::wostream& os ) const {
	os << getLeft() << "_" << getRight();
    }

} // eon


#endif
