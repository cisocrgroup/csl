#include "./MSMatch.h"
#include "../ResultSet/ResultSet.h"
#include "../Alphabet/Alphabet.h"
#include "../Global.h"

using namespace csl;

uchar w[200];

int main(int argc,char** argv) {
    if(argc < 4) {
	std::cerr<<"Use like: msFilter <alph> <comp_dic> <dlev>"<<std::endl;
	exit(1);
    }

    Alphabet alph(argv[1]);

    try {
	MSMatch matcher( alph, argv[2], atoi( argv[3] ) );
    
	ResultSet list;

	char query[200]; // = "and,artificiall,distributed,inteligence,machiene";

	while( std::cin>>query ) {
	    list.reset(); // forget candidates that might be stored from earlier use
	    matcher.query( (uchar*)query, list );

	    // print all hits
	    int i;
	    std::cout<<">>"<<query<<std::endl;
	    for(i=0;i<list.getSize();++i) {
		std::cout<<list[i].getStr()<<std::endl;
	    }
	}

    } catch( exceptions::cslException exc ) {
	std::cerr<<exc.what()<<std::endl;
    }
    
}
