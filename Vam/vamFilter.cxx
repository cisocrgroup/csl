#include<iostream>
#include "./Vam.h"
#include "../Getopt/Getopt.h"
#include "../Stopwatch.h"

int main(int argc, const char** argv ) {

    std::locale::global( std::locale( "de_DE.utf-8" ) );

    Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 3 ) {
	std::wcerr<<"Use like: vamFilter <distance> <dictionary> <pattern-file>"<<std::endl;
	exit( 1 );
    }

    csl::MinDic<> baseDic;
    baseDic.loadFromFile( opt.getArgument( 1 ).c_str() );

    csl::Vam vam( baseDic, opt.getArgument( 2 ).c_str() );

    std::vector< csl::Vam::Answer > answers;

    std::wstring query;

    vam.setDistance( atoi( opt.getArgument( 0 ).c_str() ) );
    Stopwatch watch;

    while( std::wcin >> query ) {
	watch.start();
	vam.query( query, &answers );
	std::wcout<<watch.readMilliseconds()<<" ms"<<std::endl;
    }

    
    return 0;
}
