
#include "./TestCSLLocale.h"
#include<csl/Global.h>

int main() {
    csl::TestCSLLocale t;

    try {
	t.run();
    } catch( std::exception& exc ) {
	std::wstring wideWhat;
	csl::string2wstring( exc.what(), wideWhat );
	std::wcerr << wideWhat << std::endl;
    }


}
