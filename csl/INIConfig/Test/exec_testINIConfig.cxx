

#include "./TestINIConfig.h"


int main() {

    std::locale::global( std::locale( "" ) );
    
    OCRCorrection::TestINIConfig ti;

    ti.run();

};
