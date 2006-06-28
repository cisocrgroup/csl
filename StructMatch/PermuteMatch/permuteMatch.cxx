#include "./PermuteMatch.h"
#include "../../Alphabet/Alphabet.h"
#include "../../Trie/Trie.h"
#include "../../TransTable/TransTable.h"
#include "../../MSMatch/MSMatch.h"
#include "../../LevDEA/LevDEA.h"
#include "../../Global.h"

using namespace csl;
int main(int argc, char** argv) {
    if(argc < 5) {
	std::cerr<<"Use like: permuteMatch <alph> <db_file> <dic_file> <dlev> <query_file> "<<std::endl<<std::endl;
	exit(1);
    }

    char* query_file = new char[200];
    query_file = argv[5];
    FILE* queries_handle = fopen(query_file,"r");
    if(!queries_handle) {
	std::cerr<<"Could not open query-file."<<std::endl;
	exit(1);
    }
    
    int count = 0;
    int nrOfHits;


    Alphabet alph(argv[1]);

    PermuteMatch pm(alph,argv[2], argv[3], atoi(argv[4]));
    pm.setFindParts(true);

    uchar* line = new uchar[2000];  // CAUTION!!!!
    uchar* query;
    uchar* groundtruth = NULL;
    char* sep;
    const int* results;

    while(fgets((char*)line,2000,queries_handle))  {
	line[strlen((char*)line)-1] = 0; // delete newline
	// if a tab is found, it is considered to separate groundtruth from query
	if((sep = strchr((char*)line,'\t'))) {
	    *sep = 0;
	    groundtruth = line;
	    query = (uchar*)sep + 1;
	}
	// else, whole line is considered to be the query, groundtruth remains NULL
	else {
	    query = line;
	}


	printf(">>  %s\n",query);

	nrOfHits = pm.query((uchar*)query);
//	pm.printList(); //DEBUG

	if(groundtruth) printf("gt: %s\n",groundtruth);

	results = pm.getResults();
	for(int i=0;i<nrOfHits;++i) {
	    printf("%d,",results[i]);
	}
	printf("\n---\n");

	++count;
	if(!(count % 50))
	    printf("%d queries processed\n", count);

    }
    printf("\n%d queries processed\n", count);
    fclose(queries_handle);

}
