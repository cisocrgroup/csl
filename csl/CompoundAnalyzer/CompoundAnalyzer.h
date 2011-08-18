#ifndef CSL_COMPOUNDANALYZER_H
#define CSL_COMPOUNDANALYZER_H CSL_COMPOUNDANALYZER_H

#include<csl/MinDic/MinDic.h>



namespace csl {


    class CompoundAnalyzer {
    public:
	CompoundAnalyzer();

	void loadDictionary( char const* dictFile );


	bool analyze( std::wstring const& word, size_t offset = 0 );

    private:
	MinDic<> dic_;
	
	enum {IS_FUGENFORM = 1, IS_KOPF = 2 };


    };

}

#endif
