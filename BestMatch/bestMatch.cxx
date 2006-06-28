#include "../Alphabet/Alphabet.h"
#include "../BestMatch/BestMatch.h"
#include "../ResultSet/ResultSet.h"
#include<sys/time.h>
#include<vector>

using namespace csl;

int main(int argc, char** args) {
    if(argc < 5) {
	std::cerr<<"Use like: la_test <alph> <dic> <rev_dic> <query-file>"<<std::endl;
	exit(1);
    }

    Alphabet alph(args[1]);
    BestMatch bm(alph,args[2], args[3]);

    FILE* queries_handle = fopen(args[4],"r");
    if(!queries_handle) {
	std::cerr<<"Could not open query-file."<<std::endl;
	exit(1);
    }
    
    char* line = new char[1000];  // CAUTION!!!!
    
    //bool hits;

    ResultSet candList;

    int dist;
    char* sep;

    uchar queries[10000][10000];
    uchar groundtruths[10000][10000];

    int lineCount = 0;

    while(fgets(line,1000,queries_handle))  {
	line[strlen(line)-1] = 0; // delete newline
	sep = strchr(line,'\t');
	*sep = 0;
	
	strcpy((char*)groundtruths[lineCount], line);
	strcpy((char*)queries[lineCount], (sep + 1));

	++lineCount;
    }
    fclose(queries_handle);


    int count = 0;
    
    timeval tv;
    gettimeofday(&tv, NULL);
    int begin = tv.tv_sec; 
    int ubegin = tv.tv_usec; 

    std::cout<<"Start"<<std::endl;
    while(count < lineCount)  {
	std::cout<<">>"<<queries[count]<<" ("<<groundtruths[count]<<")"<<std::endl;
      
	candList.reset();
	dist = bm.query(queries[count], candList);
	std::cout<<"Treffer im Abstand "<<dist<<std::endl;

	// std::cout<<query<<"\t"<<candList.token(0)<<"\t"<<candList.size()<<std::endl;

	for(ResultSet::Iterator it= candList.begin();it != candList.end();++it) {
	    if(!strcmp((char*)(*it).getStr(), (char*)groundtruths[count])) {
		std::cout<<*it<<" (TREFFER: "<<dist<<")"<<std::endl;
	    }
	    else {
		std::cout<<*it<<std::endl;
	    }
	}
	++count;
    }
    gettimeofday(&tv, NULL);
    int end = tv.tv_sec; 
    int uend = tv.tv_usec; 
    std::cout<<end-begin<<"."<<uend-ubegin<<" seconds for "<<count<<" queries."<<std::endl;
    
    std::cout<<std::endl<<count<<" queries processed."<<std::endl;

    delete[] line;


    return 0;
}
