#include<cstdlib>
#include "csl/MinDic/MinDic.h"
#include "csl/FBDic/FBDic.h"
#include<errno.h>

using namespace csl;

int main( int argc, char** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    try {

	if(argc != 2) {
	    std::cerr<<"Use like: "<<argv[0]<<" <binDic>"<<std::endl;
	    exit(1);
	}



	csl::MinDic<>* minDic = 0;
	csl::FBDic<>* fbdic = 0; // this one is loaded in case a fbdic is passed to the program

	// In case a .fbdic file is passed, open it and use the FWDic
	if( ( opt.getArgument( 0 ).size() >= 5 ) && opt.getArgument( 0 ).substr( opt.getArgument( 0 ).size() - 5 ) == "fbdic" ) {
	    fbdic= new csl::FBDic<>( opt.getArgument( 0 ).c_str() );
	    minDic = &( fbdic->getFWDic() );
	}
	else {
	    minDic = new csl::MinDic<>( opt.getArgument( 0 ).c_str() );
	}
	

	
	std::wstring query;
	while( std::getline( std::wcin, query ).good() ) {
	    // is this really necessary ??
	    if ( query.length() > Global::lengthOfLongStr ) {
		throw exceptions::badInput( "csl::lookupMD: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    
	    int ann = 0;
	    if( mdic->lookup( query.c_str(), &ann ) ) {
		std::wcout<<ann<<std::endl;
	    }
	    else std::wcout<<std::endl;
	}
	if( errno == EILSEQ ) {
	    throw exceptions::badInput( "csl::lookupMD: Input encoding error" );
	}
	
    } catch( exceptions::cslException ex ) {
	std::wcout<<"lookupMD: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;
}
