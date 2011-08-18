#ifndef CSL_COMPOUNDANALYZER_CXX
#define CSL_COMPOUNDANALYZER_CXX CSL_COMPOUNDANALYZER_CXX


#include "./CompoundAnalyzer.h"

namespace csl {

    CompoundAnalyzer::CompoundAnalyzer() {
    }


    void CompoundAnalyzer::loadDictionary( char const* dictFile ) {
	dic_.loadFromFile( dictFile );
    }


    bool CompoundAnalyzer::analyze( std::wstring const& word, size_t offset ) {
	typedef MinDic<>::State State_t;
	
	State_t st = dic_.getRootState();

	size_t pos = offset;
	while( pos < word.length() ) {
	    st.walk( word.at( pos ) );
	    if( st.isValid() ) {
		if( st.isFinal() ) {
		    if( st.getAnnotation() | IS_FUGENFORM ) {
			if( analyze( word, pos + 1 ) ) return true; // recursive call
		    }
		}
	    }
	    else {
		return false;
	    }
	    ++pos;
	} // for all chars
	return true;
    } // analyze


} // eon






#endif
