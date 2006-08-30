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
	levDEAs_[k_] = new LevDEA( alph_, k_ );
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
    void MSMatch< FW_BW >::intersectRight( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;
	size_t c;
	printf(" intersectRight at depth %d: %s\n", depth, word_ );

	for( c = 1; c <= alph_.size(); ++c ) {
	    if( ( newDicPos = curDict_->walk( dicPos, c ) ) && ( newLevPos = curLevDEARight_->walk( levPos, c ) ).position() != -1 ) {
		word_[depth] = alph_.decode( c );

//  word_[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		// print w if node is final in dic and lev;
		if( curDict_->isFinal( newDicPos ) && curLevDEARight_->isFinal( newLevPos ) ) {
		    word_[depth+1] = 0;
		    static uchar wordRev[Global::lengthOfWord] = "HALLO";
		    // push word and annotated value into the output list
		    if( reverse_ ) for( int i = depth, iRev = 0; i >=0; --i, ++iRev ) wordRev[iRev] = word_[i];

//		    if( ! output_->push( ( (reverse_)? wordRev : word_ ), dictFW_.getFirstAnn( newDicPos ) ) ) {// if result buffer full
		    if( ! output_->push( word_, 0 ) ) {// if result buffer full
			printf("Treffer: %s\n", ( (reverse_)? wordRev : word_ ) );
			throw exceptions::bufferOverflow( "MSMatch: ResultSet overflow for pattern: " + std::string( ( char* )pattern_ ) );
		    }
		}
		intersectRight( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    void MSMatch< FW_BW >::intersectLeft( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;
	size_t c;
	printf(" intersectLeft at depth %d: %s\n", depth, word_ );

	for( c = 1; c <= alph_.size(); ++c ) {
	    if( ( newDicPos = curDict_->walk( dicPos, c ) ) && ( newLevPos = curLevDEALeft_->walk( levPos, c ) ).position() != -1 ) {
		word_[depth] = alph_.decode( c );

//  word_[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		// print w if node is final in dic and lev;
		if( curDict_->isFinal( newDicPos ) && ( curLevDEARight_->getDist( newLevPos ) > minDistLeft_ ) ) {
		    intersectRight( newDicPos, LevDEA::Pos( 0, 0 ), depth + 1 );
		}
		intersectLeft( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    int MSMatch< FW_BW >::query( const uchar* pattern, ResultSet_if& output ) {
	output_ = &output;
	strcpy( (char*)pattern_, (const char*)pattern );
	
	// split pattern into 2 halves
	size_t patLength = strlen( (char*)pattern_ );
	size_t patCenter = (size_t) patLength / 2;
	size_t cLeft = 0, cRight = 0;	
	for ( size_t i = 0; i < patLength; ++i ) {
	    if( i < patCenter ) patLeft_[cLeft++] = pattern_[i];
	    else patRight_[cRight++] = pattern_[i];
	}
	patLeft_[cLeft] = patRight_[cRight] = 0;

	// reverse patterns
	for( int i = cLeft - 1, iRev = 0; i >=0; --i, ++iRev ) patLeftRev_[iRev] = patLeft_[i];
	for( int i = cRight - 1, iRev = 0; i >=0; --i, ++iRev ) patRightRev_[iRev] = patRight_[i];
	patLeftRev_[cLeft] = patRightRev_[cRight] = 0;

	printf("pattern=%s\npatLeft=%s\npatRight=%s\npatLeftRev=%s\npatRightRev=%s\n", pattern_, patLeft_, patRight_, patLeftRev_, patRightRev_ );

//  	// 0 | 0,1,2,3 errors
//  	reverse_ = false;
//  	curDict_ = &dictFW_;
//  	uint_t pos;
//  	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
//  	    strcpy( (char*)word_, (char*)patLeft_ );
// 	    minDistRight_ = -1;
// 	    curLevDEARight_ = levDEAs_[3];
//  	    curLevDEARight_->loadPattern( patRight_ );
//  	    intersectRight( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patLeft_ ) );
//  	}

 	// 1 | 0,1,2 errors
 	reverse_ = false;
 	curDict_ = &dictFW_;
	curLevDEALeft_ = levDEAs_[1];
	curLevDEALeft_->loadPattern( patLeft_ );
	minDistLeft_ = 1;
	curLevDEARight_ = levDEAs_[2];
	curLevDEARight_->loadPattern( patRight_ );
	minDistLeft_ = -1;
	intersectLeft( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );

	

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
    void MSMatch< STANDARD >::intersect( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;
	size_t c;
	for( c = 1; c <= alph_.size(); ++c ) {
	    if( ( newDicPos = dictFW_.walk( dicPos, c ) ) && ( newLevPos = curLevDEA_->walk( levPos, c ) ).position() != -1 ) {
		word_[depth] = alph_.decode( c );

//  word[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		// print w if node is final in dic and lev;
		if( dictFW_.isFinal( newDicPos ) && curLevDEA_->isFinal( newLevPos ) ) {
		    word_[depth+1] = 0;
		    
		    // push word and annotated value into the output list
		    if( ! output_->push( word_, dictFW_.getFirstAnn( newDicPos ) ) ) {// if result buffer full
			throw exceptions::bufferOverflow( "MSMatch: ResultSet overflow for pattern: " + std::string( ( char* )pattern_ ) );
		    }
		    
		}

		intersect( newDicPos, newLevPos, depth + 1 );
	    }
	    
	}
    }

    template<>
    int MSMatch< STANDARD >::query( const uchar* pattern, ResultSet_if& output ) {
	output_ = &output;
	strcpy( (char*)pattern_, (char*)pattern );

	curLevDEA_ = levDEAs_[k_];
	curLevDEA_->loadPattern( pattern );

// memset(stack,0,MAX_STACKSIZE * sizeof(int)); // should not be necessary

	memset( word_, 0, Global::lengthOfWord * sizeof( uchar ) ); // prepare memory for output word
	intersect( dictFW_.getRoot(), LevDEA::Pos( 0, 0 ), 0 );


	return 0;
    }

    




} // eon

#endif
