#ifndef CSL_CSLLOCALE_CXX
#define CSL_CSLLOCALE_CXX CSL_CSLLOCALE_CXX


#include "./CSLLocale.h"

namespace csl {

    CSLLocale::CSLLocale() :
	//std::locale( "" ) {
	//std::locale( std::locale( "" ), new Ctype_OldGerman( std::locale( "" ) ) ) {
	//std::locale( CSL_UTF8_LOCALE, new Ctype_OldGerman( std::locale( "" ) ) ) {
	std::locale( CSL_UTF8_LOCALE ) {
	std::wcerr << "Create CSLLocale" << std::endl;
    }

}

#endif
