

#include "./TestINIConfig.h"


int main() {

    std::locale::global( std::locale( "" ) );
    
    csl::TestINIConfig ti;

    ti.run();

};
