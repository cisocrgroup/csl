#ifndef PERMUTEMATCH_CXX
#define PERMUTEMATCH_CXX PERMUTEMATCH_CXX

#include"PermuteMatch.h"

namespace csl {

    bool PermuteMatch::staticBitsCalculated_ = false;
    bits64 PermuteMatch::b11[Global::Perm::maxNrOfTokens];


    PermuteMatch::PermuteMatch( Alphabet alph_file, char* db_file, char* dic_file, char* rev_dic_file, int dlev ) : 
	alph_( alph_file ),
	levquery_( alph_, dlev, dic_file, rev_dic_file ),
	db_( alph_, db_file ),
	list_(),
	findParts_( true ),
	minParts_( 1 ) {
	tokenDelim = alph_.code( Global::Perm::tokenDelimiter );
	compDelim = alph_.code( Global::Perm::componentDelimiter );

	// calculate the static bitstrings, if not already done
	if ( !staticBitsCalculated_ ) {
	    int i;
	    bits64 bits = 1;
	    for ( i = 0;i < Global::Perm::maxNrOfTokens; ++i ) {
		b11[i] = bits;
		bits = ( bits << 1 ) | 1;
	    }
	    staticBitsCalculated_ = true;
	}
    }

    int PermuteMatch::query( uchar* query ) {
	strncpy( ( char* )query_, ( char* )query, Global::lengthOfStr );
	if ( query_[Global::lengthOfStr-1] != 0 ) {
	    printf( "PermuteMatch: Error: query string too long.\n" );
	    exit( 1 );
	}

	uchar* c = query_;
	uchar* begin = query_;
	nrOfTokens_ = 0;
	countResults = 0;
	countCharResults = 0;
	list_.reset(); // forget candidates that might be stored from earlier use

	// parse input string
	while ( *c ) {
	    if ( *c == Global::Perm::tokenDelimiter ) {
		*c = 0;
		list_.setCurCol( nrOfTokens_ );
		levquery_.query( begin, list_ );
		begin = c + 1;
		++nrOfTokens_;
	    }
	    ++c;
	}
	list_.sort();
	list_.calcStillPossible();

	list_.printList(); //DEBUG!

	cols_full = ( 1 << nrOfTokens_ ) - 1; // a sequence of nrOfTokens_ 1-bits


	//          db_pos        w_pos list_pos  colBits   rmCand   rek_depth
	query_rec( db_.getRoot(), 0      , 0        , 0        , 0       , 0 );

	return countResults;
    }


    void PermuteMatch::query_rec( int dbPos, int w_pos, int listPos, bits32 colBits, int rightmostCand, int depth ) {
	static uchar w[Global::lengthOfStr];

// std::cout<<"depth="<<depth<<", word="<<w<<std::endl;

	////////// STOP IF ALL QUERY-TOKENS ARE USED //////////////////
	if ( depth == nrOfTokens_ ) {
	    return;
	}

	///////// CONTINUE SEARCH RECURSIVELY ////////////////////////
	int newDbPos, newNewDbPos, newRightmostCand, endof_w;
	bits32 newColBits;
	while ( listPos < list_.getSize() ) {
//     printf("Try with: %s\n",(char*)list_.at(listPos).getStr());

	    if ( !findParts_ ) { // findParts deactivated
		// if some bits left of the rightmost 1-bit are set neither in still_possible nor colBits
		if ( ( ( list_.at( listPos ).getStillPossible() | colBits ) & b11[rightmostCand] ) != b11[rightmostCand] ) {
		    std::cout << "still_possible violated: SP=" << list_.at( listPos ).getStillPossible() << ",colBits=" << colBits << ", b11=" << b11[rightmostCand] << std::endl;
		    return;
		}
	    }


	    if ( ( ( newColBits = ( colBits | list_.at( listPos ).getColBit() ) ) != colBits ) && // col not already blocked
		 ( newDbPos = db_.walkStr( dbPos, list_.at( listPos ).getStr() ) )
		 //  && printf("%d - %d\n",colBits,list_.at(listPos).getColBit())
		) { // could walk token in db

		// could walk token-delimiter in db
		if ( ( newNewDbPos = db_.walk( newDbPos, tokenDelim ) ) ) {
//      std::cout<<"could traverse in db complete token: "<<list_.at(listPos).getStr()<<std::endl;
		    strcpy( ( char* )w + w_pos, ( char* )list_.at( listPos ).getStr() );
		    endof_w = w_pos + strlen( ( char* )list_.at( listPos ).getStr() );
		    w[endof_w++] = Global::Perm::tokenDelimiter;
		    w[endof_w] = 0;

		    newRightmostCand = ( list_.at( listPos ).getCol() > rightmostCand ) ? list_.at( listPos ).getCol() :  rightmostCand;

		    query_rec( newNewDbPos, endof_w, listPos + 1, newColBits, newRightmostCand, depth + 1 );
		}

		// could walk component-delimiter in db
		if ( ( newNewDbPos = db_.walk( newDbPos, compDelim ) ) &&
		     ( findParts_ || ( newColBits == b11[depth] ) ) ) {  // if not findParts, used exactly a prefix of the query string
//      std::cout<<"could traverse in db complete token and component: "<<list_.at(listPos).getStr()<<std::endl;
		    strcpy( ( char* )w + w_pos, ( char* )list_.at( listPos ).getStr() );
		    endof_w = w_pos + strlen( ( char* )list_.at( listPos ).getStr() );
		    w[endof_w++] = Global::Perm::componentDelimiter;
		    w[endof_w] = 0;

		    newRightmostCand = ( list_.at( listPos ).getCol() > rightmostCand ) ? list_.at( listPos ).getCol() :  rightmostCand;

		    if ( findParts_ ) {
			// block all query-tokens up to rightmostCand for further use
			newColBits |= b11[newRightmostCand];
		    }

		    /////////// REPORT MATCHES ////////////////////
		    if ( db_.isFinal( newNewDbPos ) && // newnew dbPos must be a final state AND
			 ( ( findParts_ && depth >= minParts_ ) || // either the findParts-switch must be active with at least minParts tokens being processed OR ..
			   ( depth == nrOfTokens_ -1 ) ) ) { // all query-tokens must be processed
			strcpy( ( char* )charResults[countCharResults], ( char* )w );
			printf( "MATCH: %s\n", w );
			++countCharResults;

			TransTable< BASIC >::AnnIterator it( db_, newNewDbPos );
			for ( ;it.isValid();++it ) {
			    if ( countResults >= Global::Perm::maxNrOfResults ) {
				std::cerr << "PermuteMatch: Result buffer overflow. The variable Global::Perm::maxNrOfResults restricts the maximum number of results to" << Global::Perm::maxNrOfResults << ". Exiting." << std::endl;
                exit( 1 );
              }

              // dublicates can occur in list of matches, so check for them
              static int i;
              static bool seen;
              seen = false;
              for ( i = 0;i < countResults;++i ) {
                if ( results[i] == *it ) seen = true;
              }
              if ( !seen ) results[countResults++] = *it;
            }

            //    std::cout<<"Found: "<<w<<std::endl; //DEBUG!
          } // end: report matches

          query_rec( newNewDbPos, endof_w, 0, newColBits, newRightmostCand, depth + 1 );
        }
      }
      ++listPos;
    }
  }

} // eon

#endif
