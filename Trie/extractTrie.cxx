#include<iostream>

#include "../Alphabet/Alphabet.h"
#include "./Trie.h"

using namespace csl;

int main(int argc, char** argv) {
    Alphabet alph(argv[1]);
    Trie t(alph,argv[2]);

    t.printDic(0);
    
    
}
