#ifndef CSL_MSMATCH_CXX
#define CSL_MSMATCH_CXX CSL_MSMATCH_CXX

#include "./MSMatch.h"

namespace csl {

    template< MSMatchMode Mode >
    MSMatch< Mode >::MSMatch(const Alphabet& init_alphabet,char* compdic_file, int lev_distance) : 
	alph_(init_alphabet),
	dic(init_alphabet,compdic_file),
	levDEA(alph_,lev_distance) {
	
    }

    template< MSMatchMode Mode >
    MSMatch< Mode >::~MSMatch() {
    }

    template< MSMatchMode Mode >
    int MSMatch< Mode >::query(const uchar* pattern, ResultSet_if& output) {
	output_ = &output;
	pattern_ = pattern;

	levDEA.loadPattern(pattern);

//	memset(stack,0,MAX_STACKSIZE * sizeof(int)); // should not be necessary

	memset( word, 0, Global::lengthOfWord * sizeof( uchar ) ); // prepare memory for output word
	query_rec( dic.getRoot(), LevDEA::Pos( 0, 0 ), 0 );


	return 0;
    }

    template<>
    void MSMatch< STANDARD >::query_rec( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;
	int c;
	for(c = 1; c <= alph_.size(); ++c) {
	    if((newDicPos = dic.walk(dicPos,c)) && (newLevPos=levDEA.walk(levPos,c)).position()!=-1) {
		word[depth] = alph_.decode(c);

//		word[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		// print w if node is final in dic and lev; 
		if(dic.isFinal(newDicPos) && levDEA.isFinal(newLevPos)) { 
		    word[depth+1] = 0;
		    
		    // push word and annotated value into the output list
		    if( ! output_->push( word, dic.getFirstAnn( newDicPos ) ) ) {// if result buffer full
			throw exceptions::bufferOverflow( "MSMatch: ResultSet overflow for pattern: " + std::string( (char*)pattern_ ) );
		    }

		}

		query_rec(newDicPos, newLevPos, depth+1);
	    }
	    
	}
    }



} // eon

#endif
