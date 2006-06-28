#include "../Global.h"
#include "../MSMatch/MSMatch.h"
#include "../ResultSet/ResultSet.h"
#include "../Alphabet/Alphabet.h"
#include<iostream>

using namespace csl;

struct Pair {
    uchar word[Global::lengthOfWord]; // perhaps lengthOfWord would be enough ...
    int freq;
    int candscore;
};

int main(int argc, char** args) {
    Alphabet alph(args[1]);
    MSMatch ms(alph,args[2],1);
    ResultSet rset;

    if(argc!=4) {
	std::cerr<<"Use like: ./bin/getCandscore alph.azb tokens.alph.dic tokens.freq.lex"<<std::endl;
	exit(1);
    }

    int tableSize = 200000;
    Pair* table = (Pair*)malloc(tableSize * sizeof(Pair));
    memset(table,0,tableSize * sizeof(Pair));

    int count = 0;
    int nrOfWords;

    std::ifstream fileHandle(args[3]);
    if(!fileHandle.good()) {
	std::cerr<<"Couldn't open database file: "<<args[3]<<std::endl;
	exit(1);
    }

    uchar line[Global::lengthOfLongStr];
    uchar *word, *freqStr;

    // read file
    std::cerr<<"Read file ... "<<std::flush;
    while(fileHandle.getline((char*)line,Global::lengthOfLongStr))  {
	if(count >= tableSize) {
	    table = (Pair*)realloc(table,tableSize * 2 *sizeof(Pair));
	    memset(table+tableSize, 0, tableSize * sizeof(Pair));
	    tableSize *= 2;
	}
	word = line;
	freqStr = (uchar*)strchr((char*)line,(char)Global::keyValueDelimiter);
	*freqStr = 0;
	++freqStr;
	strcpy((char*)table[count].word, (char*)word);
	table[count].freq = atoi((char*)freqStr);
	table[count].candscore = 0;
	++count;
    }
    nrOfWords = count;
    std::cerr<<"ok, read "<<nrOfWords<<"lines."<<std::endl;

    for(int count=0; count<nrOfWords; ++count) {
	rset.reset();
	ms.query(table[count].word,rset);
	for(int i=0;i<rset.getSize();++i) {
	    table[rset[i].getAnn()].candscore += table[count].freq;
	}
	if(count%1000 == 0) std::cerr<<"\rProcessed "<<count<<" words."<<std::flush;
    }

    for(int count=0; count<nrOfWords; ++count) {
	printf("%s#%d\n",table[count].word,table[count].candscore);
    }

    std::cerr<<"done"<<std::endl;
    return(0);
}
