#ifndef CSL_MSMATCH_CXX
#define CSL_MSMATCH_CXX CSL_MSMATCH_CXX

#include "./MSMatch.h"

namespace csl {

    template<>
    inline MSMatch< STANDARD >::MSMatch( const Alphabet& initAlphabet, size_t k, char* compdicFile, char* ) :
	alph_( initAlphabet ),
	dictFW_( initAlphabet ),
	dictBW_( initAlphabet ), // only as dummy
	k_( k ) {
	dictFW_.loadFromFile( compdicFile );

	levDEASecond_ = new LevDEA( alph_, k_ );
	
    }

    template<>
    inline MSMatch< FW_BW >::MSMatch( const Alphabet& initAlphabet, size_t k, char* compdicFile, char* compdicRevFile ) :
	alph_( initAlphabet ),
	dictFW_( initAlphabet ),
	dictBW_( initAlphabet ),
	k_( k ) {
	dictFW_.loadFromFile( compdicFile );
	dictBW_.loadFromFile( compdicRevFile );
	levDEAFirst_ = new LevDEA( alph_, 1 );
	levDEASecond_ = new LevDEA( alph_, 1 );
    }

    template< MSMatchMode Mode >
    inline MSMatch< Mode >::~MSMatch() {}

    template< MSMatchMode Mode >
    inline void MSMatch< Mode >::intersectSecond( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;

	// store w if node is final in dic and lev;
	if( curDict_->isFinal( dicPos ) && ( levDEASecond_->getDistance( levPos ) >= minDistSecond_ ) ) {
	    word_[depth] = 0;
	    static uchar wordRev[Global::lengthOfWord];
	    // push word and annotated value into the output list
	    if( reverse_ ) {
		for( int i = depth - 1, iRev = 0; i >=0; --i, ++iRev ) wordRev[iRev] = word_[i];
		wordRev[depth] = 0;
	    }
	    // follow the word through the automaton once more to get the perfect hashing value
	    static size_t perfHashValue; static uint_t dicPos2;
	    perfHashValue = 0; dicPos2 = dictFW_.getRoot();
	    for( uchar* c = ( (reverse_)? wordRev : word_); *c; ++c ) {
		dicPos2 = dictFW_.walkPerfHash( dicPos2, alph_.code( *c ), perfHashValue );
	    }
	    output_->push( ( (reverse_)? wordRev : word_ ), dictFW_.getAnn( perfHashValue ) );
	}

	const uchar* c = curDict_->getSusoString( dicPos );
	for( ; *c; ++c ) {
	    if( ( newLevPos = levDEASecond_->walk( levPos, *c ) ).position() != -1 ) {
		newDicPos = curDict_->walk( dicPos, *c );
		assert( newDicPos ); // the transition always exists
		word_[depth] = alph_.decode( *c );
		word_[depth+1] = 0;
		// printf(" intersectSecond at depth %d: %s\n", depth, word_ );

//  word_[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		intersectSecond( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    inline void MSMatch< FW_BW >::intersectFirst( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;

	// if positions are final in dic and lev, proceed to right half of the pattern, starting
	// with the same state of the dict and the start state of the LevDEA
	if( levDEAFirst_->getDistance( levPos ) >= minDistFirst_ ) {
	    intersectSecond( dicPos, LevDEA::Pos( 0, 0 ), depth );
	}


	const uchar* c = curDict_->getSusoString( dicPos );

	for( ; *c; ++c ) {
	    if( ( newLevPos = levDEAFirst_->walk( levPos, *c ) ).position() != -1 ) {
		newDicPos = curDict_->walk( dicPos, *c );
		assert( newDicPos ); // the transition always exists
		word_[depth] = alph_.decode( *c );
		word_[depth+1] = 0;
		// printf(" intersectFirst at depth %d: %s\n", depth, word_ );

		//  word_[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		intersectFirst( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    inline void MSMatch< FW_BW >::queryCases_1() {
	uint_t pos = 0;

     	// 0 | 0,1 errors
 	reverse_ = false;
 	curDict_ = &dictFW_;
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
 	    strcpy( (char*)word_, (char*)patLeft_ );
	    minDistSecond_ = 0;
	    levDEASecond_->setDistance( 1 );
 	    levDEASecond_->loadPattern( patRight_ );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patLeft_ ) );
 	}

     	// 1 | 0 errors
 	reverse_ = true;
 	curDict_ = &dictBW_;
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patRightRev_ ) ) ) {
 	    strcpy( (char*)word_, (char*)patRightRev_ );
	    minDistSecond_ = 1;
	    levDEASecond_->setDistance( 1 );
 	    levDEASecond_->loadPattern( patLeftRev_ );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patRightRev_ ) );
 	}
    }

    template<>
    inline void MSMatch< FW_BW >::queryCases_2() {
	uint_t pos = 0;
	
	// 0 | 0,1,2 errors
 	reverse_ = false;
 	curDict_ = &dictFW_;
	// load pattern outside the if-statement: we need it anyways in the next case
	levDEASecond_->loadPattern( patRight_ );
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
 	    strcpy( (char*)word_, (char*)patLeft_ );
	    minDistSecond_ = 0;
	    levDEASecond_->setDistance( 2 );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patLeft_ ) );
 	}

 	// 1 | 0,1 errors
 	reverse_ = false;
 	curDict_ = &dictFW_;
	minDistFirst_ = 1;
	levDEAFirst_->setDistance( 1 );
	levDEAFirst_->loadPattern( patLeft_ );
	// pattern for levDEASecond is still valid from last case
	minDistSecond_ = 0;
	levDEASecond_->setDistance( 1 );
	intersectFirst( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );

	// 2 | 0 errors
 	reverse_ = true;
 	curDict_ = &dictBW_;
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patRightRev_ ) ) ) {
 	    strcpy( (char*)word_, (char*)patRightRev_ );
	    minDistSecond_ = 2;
	    levDEASecond_->setDistance( 2 );
 	    levDEASecond_->loadPattern( patLeftRev_ );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patRightRev_ ) );
 	}
    }

    template<>
    inline void MSMatch< FW_BW >::queryCases_3() {
  	uint_t pos = 0;

     	// 0 | 0,1,2,3 errors
 	reverse_ = false;
 	curDict_ = &dictFW_;
	// load pattern outside the if-statement: we need it anyways in the next case
	levDEASecond_->loadPattern( patRight_ );
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
 	    strcpy( (char*)word_, (char*)patLeft_ );
	    minDistSecond_ = 0;
	    levDEASecond_->setDistance( 3 );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patLeft_ ) );
 	}

 	// 1 | 0,1,2 errors
 	reverse_ = false;
 	curDict_ = &dictFW_;
	levDEAFirst_->setDistance( 1 );
	levDEAFirst_->loadPattern( patLeft_ );
	minDistFirst_ = 1;
	levDEASecond_->setDistance( 2 );
	// pattern for levDEASecond is still valid from last case
	minDistSecond_ = 0;
	intersectFirst( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );

	// 2,3 | 0 errors
	reverse_ = true;
	curDict_ = &dictBW_;
	// load pattern outside the if-statement: we need it anyways in the next case
	levDEASecond_->loadPattern( patLeftRev_ );
	if ( ( pos = curDict_->walkStr( curDict_->getRoot(), patRightRev_ ) ) ) {
	    strcpy( (char*)word_, (char*)patRightRev_ );
	    levDEASecond_->setDistance( 3 );
	    minDistSecond_ = 2;
	    
	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), strlen( (char*)patRightRev_ ) );
	}

	// 2 | 1 errors
 	reverse_ = true;
 	curDict_ = &dictBW_;
	levDEAFirst_->setDistance( 1 );
	levDEAFirst_->loadPattern( patRightRev_ );
	minDistFirst_ = 1;
	levDEASecond_->setDistance( 2 );
	// pattern for levDEASecond is still valid from last case
	minDistSecond_ = 2;
	intersectFirst( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );
    }


    template<>
    inline int MSMatch< FW_BW >::query( const uchar* pattern, ResultSet_if& output ) {
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

	// printf("pattern=%s\npatLeft=%s\npatRight=%s\npatLeftRev=%s\npatRightRev=%s\n", pattern_, patLeft_, patRight_, patLeftRev_, patRightRev_ );
	if( k_ == 1 ) queryCases_1();
	else if( k_ == 2 ) queryCases_2();
	else if( k_ == 3 ) queryCases_3();
	else throw exceptions::invalidLevDistance();
	return 0;
    }

    

    template<>
    inline void MSMatch< STANDARD >::intersect( int dicPos, LevDEA::Pos levPos, int depth ) {
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
		    // follow the word through the automaton once more to get the perfect hashing value
		    static size_t perfHashValue; static uint_t dicPos2;
		    perfHashValue = 0; dicPos2 = dictFW_.getRoot();
		    for( uchar* c = word_; *c; ++c ) {
			dicPos2 = dictFW_.walkPerfHash( dicPos2, alph_.code( *c ), perfHashValue );
		    }
		    output_->push( word_, dictFW_.getAnn( perfHashValue ) );
		}

		intersect( newDicPos, newLevPos, depth + 1 );
	    }
	    
	}
    }

    template<>
    inline int MSMatch< STANDARD >::query( const uchar* pattern, ResultSet_if& output ) {
	output_ = &output;
	strcpy( (char*)pattern_, (char*)pattern );

	curLevDEA_ = levDEASecond_;
	curLevDEA_->setDistance( k_ );
	curLevDEA_->loadPattern( pattern );

	memset( word_, 0, Global::lengthOfWord * sizeof( uchar ) ); // prepare memory for output word
	intersect( dictFW_.getRoot(), LevDEA::Pos( 0, 0 ), 0 );


	return 0;
    }
} // eon

#endif
