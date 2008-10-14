#include <csl/Global.h>
#include <csl/MinDicString/MinDicString.h>
#include <csl/Getopt/Getopt.h>

using namespace csl;
int main( int argc, const char** argv ) {

    std::locale::global( std::locale( "" ) );

    Getopt options( argc, argv );

    if( options.getArgumentCount() != 2 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	return 1;
    }

    MinDicString mds;

    if( options.hasOption( "cislex" ) ) {
	mds.setKeyValueDelimiter( '.' );
    }
    
    mds.compileDic( options.getArgument( 0 ).c_str() );
    mds.writeToFile( options.getArgument( 1 ).c_str() );
    
//    mds.printDic();

    return 0;
}
