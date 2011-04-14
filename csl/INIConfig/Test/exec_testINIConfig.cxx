

#include "./TestINIConfig.h"


int main() {
    std::locale::global( std::locale( "" ) );
    
    csl::TestINIConfig ti;

    try { 
	ti.run();
    } catch( std::exception& exc ) {
	std::wstring wideWhat;
	csl::string2wstring( exc.what(), wideWhat );
	std::wcout << "Exception: " << wideWhat << std::endl;
	
    }
    
};
