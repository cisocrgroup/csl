#include "../Alphabet/Alphabet.cxx"
#include "LevNDEA.cxx"
#include <iostream>
#include <cstdio>

using namespace csl;

int main(int argc, char** args) {
    if(argc < 2) {
	std::cerr<<"Use like: levNDEA_test <alph> <pattern>"<<std::endl;
	exit(1);
    }

    Alphabet alph(args[1]);
    LevNDEA lndea(alph);

    std::cout<<"LevNDEA test"<<std::endl;

    lndea.loadPattern((uchar*)args[2]);
    lndea.printFloors(0);

    char c;
    char word[100];
    int depth = 0;
    while(c = std::fgetc(stdin)) {
	if(c==10) continue;
	word[depth] = c;
	word[depth+1] = 0;
	lndea.walk(alph.code(c),depth);
	++depth;
	lndea.printFloors(depth);
	std::cout<<std::endl<<word<<std::flush;

    }





//     // print allChars
//     std::cout<<"allChars="<<std::flush;
//     const uchar* c = lndea.getAllChars();
//     while(*c) {
//       std::cout<<(char)alph.decode(*c)<<std::flush;
//       ++c;
//     }
//     std::cout<<std::endl;


//     LevAutomaton::SuggestIter sug(lndea,1);
//     // get suggestions
//     while(*sug) {
//       std::cout<<"Suggest: "<<(char)alph.decode(*sug)<<std::endl;
//       ++sug;
//     }
}
