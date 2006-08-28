#ifndef CSL_MSMATCH_CXX
#define CSL_MSMATCH_CXX CSL_MSMATCH_CXX

#include "./MSMatch.h"

namespace csl {

    template<>
    MSMatch< STANDARD >::MSMatch( const Alphabet& initAlphabet, size_t k, char* compdicFile, char* ) :
	alph_( initAlphabet ),
	dictFW_( initAlphabet, compdicFile ),
	dictBW_( initAlphabet ), // only as dummy
	k_( k ) {

	for( size_t i = 1; i <= k_; ++i ) {
	    levDEAs_[i] = new LevDEA( alph_, i );
	}
    }

    template<>
    MSMatch< FW_BW >::MSMatch( const Alphabet& initAlphabet, size_t k, char* compdicFile, char* compdicRevFile ) :
	alph_( initAlphabet ),
	dictFW_( initAlphabet, compdicFile ),
	dictBW_( initAlphabet, compdicRevFile ),
	k_( k ) {

	for( size_t i = 1; i <= k_; ++i ) {
	    levDEAs_[i] = new LevDEA( alph_, i );
	}
    }

    template< MSMatchMode Mode >
    MSMatch< Mode >::~MSMatch() {}

    template<>
    int MSMatch< FW_BW >::query( const uchar* pattern, ResultSet_if& output ) {
	strcpy( pattern_, pattern );
	
	// split pattern into 2 halves
	size_t patLength = strlen(s); 
	size_t patCenter = (size_t) patLength / 2;
	for ( size_t i = 0, size_t cLeft=0, size_t cRight=0; i < patLength; ++i ) {
	    if( i < patCenter ) patLeft_[cLeft++] = pattern_[i];
	    else patRight_[cRight++] = pattern_[i];
	}
	patLeft_[cLeft] = patRight_[cRight] = 0;

	// reverse patterns
	for( size_t i = cLeft - 1, size_t iRev = 0; i >=0; --i, ++iRev ) patLeftRev_[iRev] = patLeft_[i];
	for( size_t i = cRight - 1, size_t iRev = 0; i >=0; --i, ++iRev ) patLeftRev_[iRev] = patLeft_[i];

	// 0 | 0,1,2,3 errors
	rev_ = false;
	curDict_ = &dictFW_;
	int pos;
	if( ( pos = curDict_->walkString( curDict->getRoot(), patLeft_ ) ) ) {
	    chv = &chv3;
	    lvt = &lvt3;
	    distance2 = -1;
	    calc_charvec( p2, chv );
	    strcpy( s, p1 );
	    find_intersection3( i, 0, 0, strlen( p1 ) );
	}
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");

// 	// 1 | 0,1,2 errors
// 	chv = &chv2;
// 	lvt = &lvt2;
// 	s[0] = 0;
// 	calc_charvec( p1, chv_ );
// 	calc_charvec( p2, chv );
// 	distance1 = 0;
// 	distance2 = -1;
// 	find_intersection1( dict->initial, 0, 0, 0 );
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");

// 	// 2,3 | 0 errors
// 	rev = TRUE;
// 	dict = &rdict;
// 	if ( ( i = find_word( dict->initial, pr2 ) ) ) {
// 	    chv = &chv3;
// 	    lvt = &lvt3;
// 	    calc_charvec( pr1, chv );
// 	    strcpy( s, pr2 );
// 	    distance2 = 1;
// 	    find_intersection3( i, 0, 0, strlen( pr2 ) );
// 	}
// //for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");

// 	// 2 | 1 errors
// 	chv = &chv2;
// 	lvt = &lvt2;
// 	s[0] = 0;
// 	calc_charvec( pr2, chv_ );
// 	calc_charvec( pr1, chv );
// 	distance2 = 1;
// 	distance1 = 0;
// 	find_intersection1( dict->initial, 0, 0, 0 );
// //for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");

// 	n_all_res += nres;
// 	s[0] = 0;
// 	k1 = 0;



	return 0;
    }

    template<>
    int MSMatch< STANDARD >::query( const uchar* pattern, ResultSet_if& output ) {
	output_ = &output;
	pattern_ = pattern;

	curLevDEA_ = levDEAs_[k_];
	curLevDEA_->loadPattern( pattern );

// memset(stack,0,MAX_STACKSIZE * sizeof(int)); // should not be necessary

	memset( word, 0, Global::lengthOfWord * sizeof( uchar ) ); // prepare memory for output word
	query_rec( dic_.getRoot(), LevDEA::Pos( 0, 0 ), 0 );


	return 0;
    }

    template< MSMatchMode Mode >
    void MSMatch< Mode >::query_rec( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;
	size_t c;
	for( c = 1; c <= alph_.size(); ++c ) {
	    if( ( newDicPos = dic_.walk( dicPos, c ) ) && ( newLevPos = curLevDEA_->walk( levPos, c ) ).position() != -1 ) {
		word[depth] = alph_.decode( c );

//  word[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		// print w if node is final in dic and lev;
		if( dic_.isFinal( newDicPos ) && curLevDEA_->isFinal( newLevPos ) ) {
		    word[depth+1] = 0;

		    // push word and annotated value into the output list
		    if( ! output_->push( word, dic_.getFirstAnn( newDicPos ) ) ) {// if result buffer full
			throw exceptions::bufferOverflow( "MSMatch: ResultSet overflow for pattern: " + std::string( ( char* )pattern_ ) );
          }

        }

        query_rec( newDicPos, newLevPos, depth + 1 );
      }

    }
  }




} // eon

#endif
