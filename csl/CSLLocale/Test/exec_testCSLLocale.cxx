
#include "./TestCSLLocale.h"
#include<csl/Global.h>

int main() {
    std::locale::global( std::locale( "en_GB.utf-8" ) );

    csl::TestCSLLocale t;

    try {
	t.run();
    } catch( std::exception& exc ) {
	std::wstring wideWhat;
	csl::string2wstring( exc.what(), wideWhat );
	std::wcerr << wideWhat << std::endl;
    }


}
