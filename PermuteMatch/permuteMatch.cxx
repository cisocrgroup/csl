#include "./PermuteMatch.h"
#include <iostream>
#include "../Alphabet/Alphabet.h"

using namespace csl;

int main( int argc, char** argv ) {
    if ( argc < 6 ) {
	std::cerr << "Use like: permuteMatch <alph> <db_file> <comp_dic> <rev_comp_dic> <dlev> <query_file>" << std::endl << std::endl;
	std::cerr << "The variant permuteMatch_part also finds parts of the query. The last argument specifies that a match consists of at least <minParts> tokens of the query." << std::endl;
	std::cerr << "Use like: permuteMatch_part <alph> <dlev> <db_file> <comp_dic> <rev_comp_dic> <query_file> [<minParts>]" << std::endl;
	exit( 1 );
    }


    const Alphabet alph( argv[1] );
    char* query_file = new char[200];
    query_file = argv[6];
    FILE* queries_handle = fopen( query_file, "r" );
    if ( !queries_handle ) {
	std::cerr << "Could not open query-file." << std::endl;
	exit( 1 );
    }

    int i;
    int count = 0;
    int nrOfHits;
    PermuteMatch pm( alph,
		     atoi( argv[2] ),
		     argv[3],
		     argv[4],
		     argv[5]
	);

#ifdef FINDPARTS
    pm.setFindParts( true );
#endif

    uchar* line = new uchar[2000];  // CAUTION!!!!
    uchar* query;
    uchar* groundtruth;
    char* sep;


    while ( fgets( ( char* )line, 2000, queries_handle ) )  {
	line[strlen( ( char* )line )-1] = 0; // delete newline
	sep = strchr( ( char* )line, '\t' );
	*sep = 0;

	groundtruth = line;
	query = ( uchar* )sep + 1;

	printf( ">>  %s\n", query );

	nrOfHits = pm.query( ( uchar* )query );

// printf("gt: %s\n",groundtruth);

// pm.printList();
	for ( i = 0;i < nrOfHits;++i ) {
	    printf( ">>%d,", pm.getResults()[i] );
	}
	printf( "\n---\n" );
	++count;
	if ( !( count % 50 ) )
	    fprintf( stderr, "%d queries processed\n", count );

  }
  printf( "\n%d queries processed\n", count );
  fclose( queries_handle );
}
