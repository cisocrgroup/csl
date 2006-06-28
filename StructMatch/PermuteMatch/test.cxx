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
    query_file = argv[4];
    FILE* queries_handle = fopen(query_file,"r");
    if(!queries_handle) {
	std::cerr<<"Could not open query-file."<<std::endl;
	exit(1);
    }
    
    int count = 0;
    int nrOfHits;


    Alphabet alph(argv[1]);

    PermuteMatch pm(alph,argv[2], argv[3], atoi(argv[4]));

    uchar* line = new uchar[2000];  // CAUTION!!!!
    uchar* query;
    uchar* groundtruth;
    char* sep;
    const int* results;


    while(fgets((char*)line,2000,queries_handle))  {
	line[strlen((char*)line)-1] = 0; // delete newline
	sep = strchr((char*)line,'\t');
	*sep = 0;

	groundtruth = line;
	query = (uchar*)sep + 1;

	printf(">>  %s\n",query);

	nrOfHits = cm.query((uchar*)query);

	printf("gt: %s\n",groundtruth);

	results = cm.getResults();
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
