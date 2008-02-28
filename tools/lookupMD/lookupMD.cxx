#include "csl/MinDic/MinDic.h"

using namespace csl;

int main( size_t argc, char** argv ) {
//    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */
	std::locale::global( std::locale( "" ) ); // set the environment's default locale

    try {

	if(argc != 2) {
	    std::cerr<<"Use like: "<<argv[0]<<" <binDic>"<<std::endl;
	    exit(1);
	}

	MinDic< int > mdic;
	mdic.loadFromFile( argv[1] );

    
	std::wstring query;
	while( std::getline( std::wcin, query ).good() ) {
		for( std::wstring::const_iterator c = query.begin(); c != query.end(); ++c) {
			std::wcout<<*c<<std::endl;
		}
		// is this really necessary ??
		if ( query.length() > Global::lengthOfLongStr ) {
			throw exceptions::badInput( "csl::compileMD: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    
	    int ann = 0;
		if( mdic.lookup( query.c_str(), &ann ) ) {
			std::wcout<<query<<","<<ann<<std::endl;
	    }
	    else std::wcout<<std::endl;
	}

    } catch( exceptions::cslException ex ) {
 		std::wcout<<"lookupMD: "<<ex.what()<<std::endl;
		exit( 1 );
    }
    return 0;
}
