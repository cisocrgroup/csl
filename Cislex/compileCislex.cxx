#include "../Global.h"
#include "../Alphabet/Alphabet.h"
#include "./Cislex.h"

using namespace csl;
int main( size_t argc, char** argv ) {
    if(argc != 4) {
	std::cerr<<"Use like: "<<argv[0]<<" <alph> <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    const Alphabet alph( argv[1] );

    Cislex cislex( alph );
    
    cislex.compileDic( argv[2] );
    cislex.writeToFile( argv[3] );


    return 0;
}
