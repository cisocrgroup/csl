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
	tokenDelimiter_ = alph_.code( Global::Perm::tokenDelimiter );
	permuteDelimiter_ = alph_.code( Global::Perm::permuteDelimiter );
	noPermuteDelimiter_ = alph_.code( Global::Perm::noPermuteDelimiter );

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
		printf( "Token: %s\n", begin );
		levquery_.query( begin, list_ );
		begin = c + 1;
		++nrOfTokens_;
	    }
	    ++c;
	}
	list_.sortUnique();
	list_.calcStillPossible();
	list_.printList(); //DEBUG!

	cols_full = ( 1 << nrOfTokens_ ) - 1; // a sequence of nrOfTokens_ 1-bits


	//          db_pos        w_pos list_pos  colBits   rmCand   rek_depth
	findPermute( db_.getRoot(), 0      , 0        , 0        , 0       , 0 );

	return countResults;
    }


    void PermuteMatch::checkNewComponent( uint_t dbPos, // pos in the db-automaton
					  size_t w_pos, // end-position of the answer string ( char-wise depth )
					  size_t listPos, // pos in the candidate list
					  bits32 colBits, // bitvector indicating which query tokens are already used 
					  int rightmostCand, // the rightmost query token that is already used
					  size_t countTokens // nr of complete tokens included in the current answer
	) {

	/////////// REPORT MATCHES ////////////////////
	if ( db_.isFinal( dbPos ) && // new dbPos must be a final state AND
	     ( ( findParts_ && ( countTokens >= minParts_ ) ) || // either the findParts-switch must be active with at least minParts tokens being processed OR ..
	       ( countTokens == nrOfTokens_ ) ) ) { // all query-tokens must be processed
	    strcpy( ( char* )charResults[countCharResults], ( char* )w_ );
	    printf( "MATCH: %s\n", w_ );
	    ++countCharResults;
	    
	    TransTable< BASIC >::AnnIterator it( db_, dbPos );
	    for ( ;it.isValid();++it ) {
		if ( countResults >= Global::Perm::maxNrOfResults ) {
		    throw exceptions::bufferOverflow( "PermuteMatch: Result buffer overflow. The variable Global::Perm::maxNrOfResults restricts the maximum number of results." );
		}
		
		// dublicates can occur in list of matches, so check for them
		static int i;
		static bool seen;
		seen = false;
		for ( i = 0; i < countResults; ++i ) {
		    if ( results[i] == *it ) seen = true;
		}
		if ( !seen ) results[countResults++] = *it;
	    }
	    
	    //    std::cout<<"Found: "<<w_<<std::endl; //DEBUG!
	} // end: report matches
	

	////// TRY TO WALK THE PART-DELIMITERS ///////////////////////
	walkedPermDelim = db_.walk( dbPos, permuteDelimiter_ );
	walkedNoPermDelim = db_.walk( dbPos, noPermuteDelimiter_ );
	if ( ( walkedPermDelim || walkedNoPermDelim ) &&
	     ( findParts_ || ( colBits == b11[countTokens] ) ) ) {  // if not findParts, used exactly a prefix of the query string
// 	    strcpy( ( char* )w_ + w_pos, ( char* )list_.at( listPos ).getStr() );
// 	    new_w_pos = w_pos + strlen( ( char* )list_.at( listPos ).getStr() );
	    
	    if ( findParts_ && ( rightmostCand > 0 ) ) {
		// block all query-tokens up to rightmostCand for further use
		newColBits |= b11[rightmostCand];
	    }
	    newRightmostCand = std::max( (int)list_.at( listPos ).getCol(), rightmostCand );
	    
	    if( walkedPermDelim ) {
		new_w_pos = w_pos;
		w_[new_w_pos] = Global::Perm::permuteDelimiter;
		++new_w_pos;
		w_[new_w_pos] = 0;
		std::cout<<"->permuteDelimiter - pos="<<walkedPermDelim<<", w="<<w_<<std::endl;

		findPermute( walkedPermDelim, new_w_pos, 0, newColBits, newRightmostCand, countTokens );
	    }
	    if( walkedNoPermDelim ) {
		new_w_pos = w_pos;
		w_[new_w_pos] = Global::Perm::noPermuteDelimiter;
		++new_w_pos;
		w_[new_w_pos] = 0;
		findPermute( walkedNoPermDelim, new_w_pos, 0, newColBits, newRightmostCand, countTokens );
	    }
	} // could walk compDelim

    } // checkNewComponent

    void PermuteMatch::findSequence( uint_t dbPos, // pos in the db-automaton
				    size_t w_pos, // end-position of the answer string ( char-wise depth )
				    size_t listPos, // pos in the candidate list
				    bits32 colBits, // bitvector indicating which query tokens are already used 
				    int rightmostCand, // the rightmost query token that is already used
				    size_t countTokens // nr of complete tokens included in the current answer
	) {
	std::cout<<"Enter findSequence: countTokens="<<countTokens<<", word="<<w_<<", dbPos="<<dbPos<<std::endl;
	
	checkNewComponent( dbPos, w_pos, listPos, colBits, rightmostCand, countTokens );

	
	
    }

    /**
     * This recursive function handles the permutation inside parts when permutation among parts is NOT allowed
     *
     * @param dbPos pos in the db-automaton
     * @param w_pos end-position of the answer string ( char-wise depth )
     * @param listPos pos in the candidate list
     * @param colBits bitvector indicating which query tokens are already used 
     * @param rightmostCand the rightmost query token that is already used
     * @param depth nr of complete tokens included in the current answer
     */
    void PermuteMatch::findPermute( uint_t dbPos, // pos in the db-automaton
				    size_t w_pos, // end-position of the answer string ( char-wise depth )
				    size_t listPos, // pos in the candidate list
				    bits32 colBits, // bitvector indicating which query tokens are already used 
				    int rightmostCand, // the rightmost query token that is already used
				    size_t countTokens // nr of complete tokens included in the current answer
	) {

	std::cout<<"Enter findPermute: countTokens="<<countTokens<<", word="<<w_<<", dbPos="<<dbPos<<std::endl;

	checkNewComponent( dbPos, w_pos, listPos, colBits, rightmostCand, countTokens );
	
	////////// STOP IF ALL QUERY-TOKENS ARE USED //////////////////
	if ( countTokens == nrOfTokens_ ) {
	    return;
	}


	uint_t walkedPermDelim, walkedNoPermDelim;
	int newRightmostCand;
	size_t new_w_pos;
	bits32 newColBits = 0;


	

	///////// TRY TO WALK TOKENS ///////////////////////////
	uint_t newDbPos, walkedTokenDelim;
	while ( listPos < list_.getSize() ) {
	    printf("Try with: %s from dbPos %d\n",(char*)list_.at(listPos).getStr(), dbPos );

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
		if ( ( walkedTokenDelim = db_.walk( newDbPos, tokenDelimiter_ ) ) ) {
//      std::cout<<"could traverse in db complete token: "<<list_.at(listPos).getStr()<<std::endl;
		    strcpy( ( char* )w_ + w_pos, ( char* )list_.at( listPos ).getStr() );
		    new_w_pos = w_pos + strlen( ( char* )list_.at( listPos ).getStr() );
		    w_[new_w_pos++] = Global::Perm::tokenDelimiter;
		    w_[new_w_pos] = 0;

		    newRightmostCand = std::max( (int)list_.at( listPos ).getCol(), rightmostCand );
		    
		    findPermute( walkedTokenDelim, new_w_pos, listPos + 1, newColBits, newRightmostCand, countTokens + 1 );
		}
		
		

	    }
	    ++listPos;
	}
    } // findPermute
    
    
    /**
     * This recursive function handles the permutation inside parts AND among parts
     *
     * @param dbPos pos in the db-automaton
     * @param w_pos end-position of the answer string ( char-wise depth )
     * @param listPos pos in the candidate list
     * @param colBits bitvector indicating which query tokens are already used 
     * @param rightmostCand the rightmost query token that is already used
     * @param countTokens nr of complete tokens included in the current answer
     */
//     void PermuteMatch::findPermute_compPerm( uint_t dbPos, // pos in the db-automaton
// 				     size_t w_pos, // end-position of the answer string ( char-wise depth )
// 				     size_t listPos, // pos in the candidate list
// 				     bits32 colBits, // bitvector indicating which query tokens are already used 
// 				     size_t rightmostCand, // the rightmost query token that is already used
// 				     size_t countTokens // nr of complete tokens included in the current answer
// 	) {
// 	static uchar w[Global::lengthOfStr];

// 	// std::cout<<"countTokens="<<countTokens<<", word="<<w<<std::endl;

// 	////////// STOP IF ALL QUERY-TOKENS ARE USED //////////////////
// 	if ( countTokens == nrOfTokens_ ) {
// 	    return;
// 	}

// 	///////// CONTINUE SEARCH RECURSIVELY ////////////////////////
// 	int newDbPos, newNewDbPos, newRightmostCand, new_w_pos;
// 	bits32 newColBits;
// 	while ( listPos < list_.getSize() ) {
// //     printf("Try with: %s\n",(char*)list_.at(listPos).getStr());

// 	    if ( !findParts_ ) { // findParts deactivated
// 		// if some bits left of the rightmost 1-bit are set neither in still_possible nor colBits
// 		if ( ( ( list_.at( listPos ).getStillPossible() | colBits ) & b11[rightmostCand] ) != b11[rightmostCand] ) {
// 		    std::cout << "still_possible violated: SP=" << list_.at( listPos ).getStillPossible() << ",colBits=" << colBits << ", b11=" << b11[rightmostCand] << std::endl;
// 		    return;
// 		}
// 	    }
	    
// 	    if ( ( ( newColBits = ( colBits | list_.at( listPos ).getColBit() ) ) != colBits ) && // col not already blocked
// 		 ( newDbPos = db_.walkStr( dbPos, list_.at( listPos ).getStr() ) )
// 		 //  && printf("%d - %d\n",colBits,list_.at(listPos).getColBit())
// 		) { // could walk token in db

// 		// could walk token-delimiter in db
// 		if ( ( newNewDbPos = db_.walk( newDbPos, tokenDelimiter ) ) ) {
// //      std::cout<<"could traverse in db complete token: "<<list_.at(listPos).getStr()<<std::endl;
// 		    strcpy( ( char* )w + w_pos, ( char* )list_.at( listPos ).getStr() );
// 		    new_w_pos = w_pos + strlen( ( char* )list_.at( listPos ).getStr() );
// 		    w[new_w_pos++] = Global::Perm::tokenDelimiteriter;
// 		    w[new_w_pos] = 0;

// 		    newRightmostCand = ( list_.at( listPos ).getCol() > rightmostCand ) ? list_.at( listPos ).getCol() :  rightmostCand;

// 		    findPermute( newNewDbPos, new_w_pos, listPos + 1, newColBits, newRightmostCand, countTokens + 1 );
// 		}

// 		// could walk component-delimiter in db
// 		if ( ( newNewDbPos = db_.walk( newDbPos, compDelimiter ) ) &&
// 		     ( findParts_ || ( newColBits == b11[countTokens] ) ) ) {  // if not findParts, used exactly a prefix of the query string
// //      std::cout<<"could traverse in db complete token and component: "<<list_.at(listPos).getStr()<<std::endl;
// 		    strcpy( ( char* )w + w_pos, ( char* )list_.at( listPos ).getStr() );
// 		    new_w_pos = w_pos + strlen( ( char* )list_.at( listPos ).getStr() );
// 		    w[new_w_pos++] = Global::Perm::componentDelimiter;
// 		    w[new_w_pos] = 0;

// 		    newRightmostCand = ( list_.at( listPos ).getCol() > rightmostCand ) ? list_.at( listPos ).getCol() :  rightmostCand;

// 		    if ( findParts_ ) {
// 			// block all query-tokens up to rightmostCand for further use
// 			newColBits |= b11[newRightmostCand];
// 		    }

// 		    /////////// REPORT MATCHES ////////////////////
// 		    if ( db_.isFinal( newNewDbPos ) && // newnew dbPos must be a final state AND
// 			 ( ( findParts_ && countTokens >= minParts_ ) || // either the findParts-switch must be active with at least minParts tokens being processed OR ..
// 			   ( countTokens == nrOfTokens_ -1 ) ) ) { // all query-tokens must be processed
// 			strcpy( ( char* )charResults[countCharResults], ( char* )w );
// 			printf( "MATCH: %s\n", w );
// 			++countCharResults;
			
// 			TransTable< BASIC >::AnnIterator it( db_, newNewDbPos );
// 			for ( ;it.isValid();++it ) {
// 			    if ( countResults >= Global::Perm::maxNrOfResults ) {
// 				throw exceptions::bufferOverflow( "PermuteMatch: Result buffer overflow. The variable Global::Perm::maxNrOfResults restricts the maximum number of results." );
// 			    }
// 			    // dublicates can occur in list of matches, so check for them
// 			    static int i;
// 			    static bool seen;
// 			    seen = false;
// 			    for ( i = 0;i < countResults;++i ) {
// 				if ( results[i] == *it ) seen = true;
// 			    }
// 			    if ( !seen ) results[countResults++] = *it;
// 			}
			
// 			//    std::cout<<"Found: "<<w<<std::endl; //DEBUG!
// 		    } // end: report matches
		    
// 		    find_permute( newNewDbPos, new_w_pos, 0, newColBits, newRightmostCand, countTokens + 1 );
// 		} // could walk comp delimititer
// 	    }
// 	    ++listPos;
// 	}
//     }
    
} // eon

#endif
