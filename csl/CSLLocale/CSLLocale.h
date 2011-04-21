#ifndef CSL_CSLLOCALE_H
#define CSL_CSLLOCALE_H CSL_CSLLOCALE_H

#include<locale>
#include<csl/codecvt/UTF8_Codecvt.h>
#include<csl/Locale/Ctype_OldGerman.h>

namespace csl {

    class CSLLocale : public std::locale {
    public:
	inline static CSLLocale& Instance() {
	    static CSLLocale cslLocale_;  // local static object initialization
	    return cslLocale_;
	}
    private:
	CSLLocale();

	CSLLocale( CSLLocale const & );

	inline ~CSLLocale() { 
	}  

	CSLLocale &operator=( CSLLocale const & );  // assignment operator is hidden

    };

	
} // eon






#endif
