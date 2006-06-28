#include "./PermuteMatch.h"
#include<iostream>

using namespace csl;
int main(int argc, char** argv) {
    if(argc < 5) {
	std::cerr<<"Use like: permuteMatch <alph> <db_file> <comp_dic> <dlev> <query_file>"<<std::endl<<std::endl;
	std::cerr<<"The variant permuteMatch_part also finds parts of the query. The last argument specifies that a match consists of at least <minParts> tokens of the query."<<std::endl;
	std::cerr<<"Use like: permuteMatch_part <alph> <db_file> <comp_dic> <dlev> <query_file> [<minParts>]"<<std::endl;
	exit(1);
    }

    char* query_file = new char[200];
    query_file = argv[5];
    FILE* queries_handle = fopen(query_file,"r");
    if(!queries_handle) {
	std::cerr<<"Could not open query-file."<<std::endl;
	exit(1);
    }
    
    int i;
    int count = 0;
    int nrOfHits;
    PermuteMatch pm(argv[1],
		     argv[2],
		     argv[3],
		     atoi(argv[4]));

#ifdef FINDPARTS
    pm.setFindParts(true);
#endif

    uchar* line = new uchar[2000];  // CAUTION!!!!
    uchar* query;
    uchar* groundtruth;
    char* sep;
    

    while(fgets((char*)line,2000,queries_handle))  {
	line[strlen((char*)line)-1] = 0; // delete newline
	sep = strchr((char*)line,'\t');
	*sep = 0;

	groundtruth = line;
	query = (uchar*)sep + 1;

	printf(">>  %s\n",query);

	nrOfHits = pm.query((uchar*)query);

//	printf("gt: %s\n",groundtruth);

//	pm.printList();
	for(i=0;i<nrOfHits;++i) {
	    printf(">>%d,",pm.getResults()[i]);
	}
	printf("\n---\n");
	++count;
	if(!(count % 50))
	    fprintf(stderr,"%d queries processed\n", count);

    }
    printf("\n%d queries processed\n", count);
    fclose(queries_handle);
}
