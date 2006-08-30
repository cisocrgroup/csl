#include "./MSMatch.h"
#include "../ResultSet/ResultSet.h"
#include "../Alphabet/Alphabet.h"
#include "../Global.h"

using namespace csl;

uchar w[200];

int main(int argc,char** argv) {
    if( argc < 5 ) {
	std::cerr<<"Use like: msFilter <alph> <dlev> <comp_dic> <rev_comp_dic>"<<std::endl;
	exit(1);
    }

    Alphabet alph(argv[1]);

    try {
	MSMatch< STANDARD > stdMatcher( alph, atoi( argv[2] ), argv[3] );
	MSMatch< FW_BW > matcher( alph, atoi( argv[2] ), argv[3], argv[4] );
    
	ResultSet stdList;
	ResultSet list;

	char query[200]; // = "and,artificiall,distributed,inteligence,machiene";

	while( std::cin>>query ) {
	    list.reset(); // forget candidates that might be stored from earlier use
	    matcher.query( (uchar*)query, list );

	    stdList.reset(); // forget candidates that might be stored from earlier use
	    stdMatcher.query( (uchar*)query, stdList );

	    // print all hits
	    int i;
	    std::cout<<">>"<<query<<std::endl;
	    for(i=0;i<list.getSize();++i) {
		std::cout<<list[i].getStr()<<std::endl;
		if( strcmp( (char*)list[i].getStr(), (char*)stdList[i].getStr() ) ) {
		    printf( "Trouble  here!!\n" );
		}
	    }
	}

    } catch( exceptions::cslException exc ) {
	std::cerr<<exc.what()<<std::endl;
    }
    
}
