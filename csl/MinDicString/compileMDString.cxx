#include "../Global.h"
#include "./MinDicString.h"

using namespace csl;
int main( size_t argc, char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */


    if(argc != 3) {
	std::cerr<<"Use like: "<<argv[0]<<" <txtDic> <binDic>"<<std::endl;
	exit(1);
    }

    MinDicString mdic;
    mdic.setKeyValueDelimiter( '#' );
    
    mdic.compileDic( argv[1] );
    mdic.writeToFile( argv[2] );
    
//    mdic.printDic();

    return 0;
}
