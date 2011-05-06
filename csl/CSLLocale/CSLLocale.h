#ifndef CSL_CSLLOCALE_H
#define CSL_CSLLOCALE_H CSL_CSLLOCALE_H

#include<locale>
#include<sstream>
#include<csl/Global.h>
#include<csl/codecvt/UTF8_Codecvt.h>
#include<csl/Locale/Ctype_OldGerman.h>

namespace csl {

    /**
     * @brief This class represents 
     */
    class CSLLocale : public std::locale {
    public:
	inline static CSLLocale& Instance() {
	    static CSLLocale cslLocale_;  // local static object initialization
	    return cslLocale_;
	}

	/**
	 * @brief Goes the STL facet-based way to transform a string to a number
	 * @param[in]  str     a string containing the number and nothing else
	 *
	 * Throws a cslException if the conversion causes problems.
	 */
	template< typename T, typename char_type >
	inline static T string2number( std::basic_string< char_type > const& str ) {
	    T number;
	    std::basic_istringstream< char_type > iss;
	    iss.imbue( Instance() );
	    iss.str( str );

	    iss >> number;
	    
	    // no badbit, no failbit, no goodbit: only the eofbit, because the complete string should be interpreted
	    if( iss.rdstate() != (  std::wistringstream::eofbit ) ) {
		throw exceptions::cslException( "csl::CSLLocale::string2number: could not interpret string." );
	    }
	    return number;
	}

    private:
	/**
	 * @brief private constructor
	 */
	CSLLocale();

	/**
	 * @brief private destructor
	 */
	CSLLocale( CSLLocale const & );

	inline ~CSLLocale() { 
	}  

	CSLLocale &operator=( CSLLocale const & );  // assignment operator is hidden

    };

	
} // eon






#endif
