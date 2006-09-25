#include<iostream>

#include "./PermuteMatch.h"
#include "../../Alphabet/Alphabet.h"
#include "../../MinDic/MinDic.h"
#include "../../TransTable/TransTable.h"
//#include "../../MSMatch/MSMatch.h"
#include "../../LevDEA/LevDEA.h"
#include "../../Global.h"

using namespace csl;

//int main( int argc, char** argv ) {
int main() {
    printf( "Test\n" );
    
//     if ( argc != 7 ) {
// 	std::cerr << "Use like: permuteMatch <alph> <db_file> <dic_file> <rev_dic_file> <dlev> <query_file> " << std::endl << std::endl;
// 	exit( 1 );
//     }

    try {
	char* query_file = new char[200];
	query_file = "scheisse.qu";
	FILE* queries_handle = fopen( query_file, "r" );
	if ( !queries_handle ) {
	    std::cerr << "Could not open query-file." << std::endl;
	    exit( 1 );
	}
	int count = 0;
	int nrOfHits;


	Alphabet alph( "scheisse.azb" );
	
	PermuteMatch pm( alph, "", "", "", 1 );
//	PermuteMatch pm( alph, argv[2], argv[3], argv[4], atoi( argv[5] ) );
//	pm.setFindParts( true );





    } 
    catch( exceptions::cslException ex ) {
	std::cout<<"permuteMatch: "<<ex.what()<<std::endl;
    }


    return 0;
}
