#include<iostream>
#include <csl/Getopt/Getopt.h>
#include <csl/Alphabet/Alphabet.h>
#include <csl/FBDic/FBDic.h>

using namespace csl;

int main( int argc, const char** argv ) {
    // setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
    std::locale::global( CSL_UTF8_LOCALE );
    std::wcout.imbue(CSL_UTF8_LOCALE);

    Getopt options( argc, argv );


    if( options.getArgumentCount() != 1 ) {
	std::wcerr<<std::endl
		 <<"Use like: extractMD [options] <binDic> "<<std::endl
		 <<"Options:"<<std::endl
		 <<"--stat=1\tto print statistics of the MinDic"<<std::endl
		 <<"--dot=1\tto print dotcode for the MinDic"
		 <<std::endl<<std::endl;

	exit(1);
    }
    try {
	FBDic<> t;
	t.loadFromFile( options.getArgument( 0 ).c_str() );

	if( options.hasOption( "dot" ) ) {
	    t.getFWDic().toDot();
	}
	else if( options.hasOption( "stat" ) ) {
	    std::wcerr << "This option is currently not implemented for FBDic"<< std::endl;
	    
	}
	else t.getFWDic().printDic();

	return 0;
    } catch ( exceptions::cslException ex ) {
	std::wcerr << "Dictionary extraction failed: " << ex.what() << std::endl;
	return 1;
    }
}
