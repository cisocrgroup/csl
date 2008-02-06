
#include "./Vam.h"
#include "../Getopt/Getopt.h"

int main(int argc, const char** argv ) {

    std::locale::global( std::locale( "de_DE.utf-8" ) );

    Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 2 ) {
	std::wcerr<<"so nicht!"<<std::endl;
	exit( 1 );
    }

    csl::MinDic<> baseDic;
    baseDic.loadFromFile( opt.getArgument( 0 ).c_str() );

    csl::Vam vam( baseDic, opt.getArgument( 1 ).c_str() );

    std::vector< csl::Vam::Answer > answers;

    const std::wstring query( L"test" );

    vam.setDistance( 1 );
    vam.query( query, &answers );
    
    return 0;
}
