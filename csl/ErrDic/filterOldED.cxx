#include<iostream>
#include<fstream>

#include "../MinDic/MinDic.h"

int main( int argc, char** argv ) {
    
    std::locale::global( std::locale( "" ) );
    if( argc < 2 ) {
	std::wcout<<"Use like: filterOldED filterLex.mdic [-1]"<<std::endl;
	exit( 1 );
    }

    csl::MinDic<> filter( argv[1] );


    bool falseFriends = false;
    if( argc == 3 && ! strcmp(argv[2],"-1") ) falseFriends = true;
    
    std::wstring line;
    size_t count = 0;
    while( std::getline( std::wcin, line ).good() ) {
	if( ! (++count % 100000) ) std::wcerr<<count / 1000 <<"k lines processed"<<std::endl;
	size_t sep = line.find( ' ' );

	if( filter.lookup( line.substr( 0, sep ).c_str() ) ) {
	    if( falseFriends ) {
		std::wcout<<line<<std::endl;
	    }
	}
	else {
	    if( ! falseFriends ) {
		std::wcout<<line<<std::endl;
	    }
	}
    }
}
