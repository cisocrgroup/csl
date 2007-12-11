
#include<list>
#include<fstream>
#include<locale>

#include "../MinDic/MinDic.h"
#include "./PatternApplier.h"
#include "./ErrDic.h"
#include "../Stopwatch.h"

namespace csl {

    class ErrDicConstructor {
    public:

	void constructErrDic( const MinDic< int >& dic_, const MinDic< int >& filterDic_, const char* patternFile, ErrDic& errDic );

    private:
	typedef PatternApplier::ErrorIterator_t ErrorIterator_t;
    };


    void ErrDicConstructor::constructErrDic( const MinDic< int >& dic, const MinDic< int >& filterDic, const char* patternFile, ErrDic& errDic ) {
	
	Stopwatch watch;
	watch.start();
	PatternApplier applier( dic, filterDic, patternFile );

	try {

	    errDic.initConstruction();
	    size_t nrOfTokens = 0;

	    while( applier.isGood() ) {
//		std::wcout<<applier.getWord()<<", "<<applier.getPattern()<<","<<applier.getErrorPos()<<std::endl;
//		applier.printCurrent( std::wcout );

		if( ! ( ++nrOfTokens % 100000 ) ) {
		    std::wcerr<<nrOfTokens / 1000<<"k. "<<watch.readMilliseconds()<<" ms"<< std::endl;
		    watch.start();
		    applier.printCurrent( std::wcerr );

		}

//		errDic.addToken( applier.getWord().c_str(), L"elefant", applier.getPattern() );
		
		applier.next();
	    }
	    
	    errDic.finishConstruction();
	    
	} catch( exceptions::badInput exc ) {
	    std::wcout<<"ErrDic Creation failed: "<<exc.what()<<std::endl; 
	}
    }

} // eon
