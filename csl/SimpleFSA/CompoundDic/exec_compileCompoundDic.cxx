#include "./CompoundDic.h"
#include<csl/TransTable/TransTable.h>
#include<csl/SimpleFSA/Simple2TransTable.h>

int main( int argc, char** argv ) {
    
    std::locale::global( std::locale( "de_DE.utf-8" ) );

    csl::SimpleFSA::CompoundDic cd;

    cd.compute( argv[1] );

    cd.getDic().analyze();

    csl::TransTable< csl::TOKDIC > transTable;
    csl::SimpleFSA::Simple2TransTable converter;

    converter.translate( cd.getDic(), &transTable );

    transTable.createBinary( (char const*)"test.dic" );

}

