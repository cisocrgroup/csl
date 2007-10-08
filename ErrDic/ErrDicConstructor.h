
#include<list>
#include<fstream>
#include<locale>

#include "../MinDic/MinDic.h"
#include "./PatternApplier.h"
#include "./ErrDic.h"

namespace csl {

    class ErrDicConstructor {
    public:

	void constructErrDic( const MinDic< int >& dic_, const MinDic< int >& filterDic_, const char* patternFile, ErrDic& errDic );

    private:
    };



    void ErrDicConstructor::constructErrDic( const MinDic< int >& dic, const MinDic< int >& filterDic, const char* patternFile, ErrDic& errDic ) {

	PatternApplier applier( dic, filterDic, patternFile );

	try {

	    errDic.initConstruction();
	    size_t nrOfTokens = 0;

	    while( applier.isGood() ) {
		std::wcout<<applier.getWord()<<", "<<applier.getPHValue()<<std::endl;
		if( ! ( ++nrOfTokens % 100000 ) ) std::wcerr<<nrOfTokens / 1000<<"k"<<std::endl;

//		errDic.addToken( applier.getWord().c_str(), L"elefant", L"u l" );

		applier.next();
	    }
	    
	    errDic.finishConstruction();

	} catch( exceptions::badInput exc ) {
	    std::wcout<<"ErrDic Creation failed: "<<exc.what()<<std::endl; 
	}
    }

} // eon
