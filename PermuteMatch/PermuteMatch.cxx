#ifndef PERMUTEMATCH_CXX
#define PERMUTEMATCH_CXX PERMUTEMATCH_CXX

#include"PermuteMatch.h"

namespace csl {

    PermuteMatch::PermuteMatch( const Alphabet& alph, int dlev, char* db_file, char* dic_file, char* rev_dic_file ) :
	alph_( alph ),
	levquery_( alph, dlev, dic_file, rev_dic_file ),
	db( alph, db_file ),
	list_(),
	findParts( false ),
	minParts( 1 ) {
    }
    int PermuteMatch::query( uchar* query ) {
	uchar* begin = query;
	nrOfTokens = 0;
	countResults = 0;
	countCharResults = 0;
	list_.reset(); // forget candidates that might be stored from earlier use

	while ( *query ) {
	    if ( *query == Global::Perm::tokenDelimiter ) {
		*query = 0;
		list_.setCurCol( nrOfTokens );
		levquery_.query( begin, list_ );
		begin = query + 1;
		++nrOfTokens;
	    }
	    ++query;
	}
	list_.sort();
	list_.calcStillPossible();
//    list_.print(); //DEBUG!

	cols_full = ( 1 << nrOfTokens ) - 1; // a sequence of nrOfTokens 1-bits

	//          db_pos        w_pos list_pos  col_bits  rek_depth
	query_rec( db.getRoot(), 0      , 0        , 0,        0 );

	return countResults;
    }

    void PermuteMatch::query_rec( int db_pos, int w_pos, int list_pos, bits32 col_bits, int depth ) {
// std::cout<<"rek_query(db_pos="<<db_pos<<",w_pos="<<w_pos<<",col_bits="<<col_bits<<",depth="<<depth<<")"<<std::endl;
	static uchar w[Global::lengthOfStr];

	// report hits
	if ( db.isFinal( db_pos ) && // db_pos must be a final state AND
	     ( ( findParts && depth >= minParts ) || // either the findParts-switch must be active with at least minParts tokens being processed OR ..
	       depth == nrOfTokens ) ) { // all query-tokens must be processed
	    strcpy( ( char* )charResults[countCharResults], ( char* )w );
	    ++countCharResults;

	    TransTable< BASIC >::AnnIterator it( db, db_pos );
	    for ( ;it.isValid();++it ) {
		if ( countResults >= Global::Perm::maxNrOfResults ) {
		    std::cerr << "PermuteMatch: Result buffer overflow. The variable Global::Perm::maxNrOfResults restricts the maximum number of results to" << Global::Perm::maxNrOfResults << ". Exiting." << std::endl;
		    exit( 1 );
		}
		static int i;
		static bool seen;
		seen = false;
		for ( i = 0;i < countResults;++i ) {
		    if ( results[i] == *it ) seen = true;
		}
		if ( !seen ) results[countResults++] = *it;
	    }

	    //    std::cout<<"Found: "<<w<<std::endl; //DEBUG!
	}
	if ( depth == nrOfTokens ) {
	    return;
	}

	int new_db_pos, endof_w;
	bits32 newColBits;
	while ( list_pos < list_.getSize() ) {
//     std::cout<<std::endl<<"Try '"<<list_.at(list_pos).getStr()<<"' at depth "<<depth<<", col_bits="<<col_bits<<": "<<std::flush;

	    if ( !findParts && ( list_.at( list_pos ).getStillPossible() | col_bits ) != cols_full ) {// if some bits are set neither in still_possible nor col_bits
//  std::cout<<"still_poosible violated: "<<std::endl;
		return;
	    }

	    if ( ( ( newColBits = ( col_bits | list_.at( list_pos ).getCol() ) ) != col_bits ) && // col not already blocked
		 //        (std::cout<<"col still open, "<<std::flush) &&
		 ( new_db_pos = db.walkStr( db_pos, list_.at( list_pos ).getStr() ) ) &&  // could walk token in db
		 ( new_db_pos = db.walk( new_db_pos, alph_.code( Global::Perm::tokenDelimiter ) ) ) && // could walk delimiter in db
		 // (std::cout<<"could traverse in db, OK"<<std::flush) &&
		 1 ) {
        strcpy( ( char* )w + w_pos, ( char* )list_.at( list_pos ).getStr() );
        endof_w = w_pos + strlen( ( char* )list_.at( list_pos ).getStr() );
        w[endof_w] = Global::Perm::tokenDelimiter;
        ++endof_w;
        w[endof_w] = 0;
        query_rec( new_db_pos, endof_w, list_pos + 1, newColBits, depth + 1 );
      }
      ++list_pos;
    }
  }

} // eon

#endif
