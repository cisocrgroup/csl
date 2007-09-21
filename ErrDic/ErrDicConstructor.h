
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
	std::list< PatternApplier > list_;
    };

    void ErrDicConstructor::constructErrDic( const MinDic< int >& dic, const MinDic< int >& filterDic, const char* patternFile, ErrDic& errDic ) {

	std::wifstream fi;
	fi.imbue( std::locale( "de_DE.UTF-8" ) );
	fi.open( patternFile );

	std::wstring line;
	while( getline( fi, line ) ) {
	    size_t delimPos = line.find( ' ' );
	    if( delimPos == std::wstring::npos ) {
		throw exceptions::badInput( "ErrDicConstructor: Invalid line in pattern file" );
	    }
	    list_.push_back( PatternApplier( dic, filterDic, line.substr( 0, delimPos ), line.substr( delimPos + 1 ) ) );
	    if( ! list_.back().isGood() ) list_.pop_back();
	    std::wcerr<<"New Applier: "<<list_.back().getWord()<<std::endl;

	    if( list_.size() % 100 == 0 ) std::wcerr<<"Processed "<<list_.size()<<" patterns"<<std::endl;

	}
	fi.close();
	list_.sort();
	std::wcerr<<"Loaded "<<list_.size()<<" patterns with s.th. to say"<<std::endl;

	try {

	    errDic.initConstruction();
	    size_t nrOfTokens = 0;
	    
	    while( list_.begin() != list_.end() ) {
		PatternApplier& pa = *( list_.begin() );
		

		std::wstring pattern = pa.getPatternFrom() + L" " + pa.getPatternTo();

		std::wcout<<pa.getWord()<<" "<<pattern<<std::endl;
//		errDic.addToken( pa.getWord().c_str(), L"test", pattern.c_str() );

		if( ++nrOfTokens % 100000 == 0 ) std::wcerr<< int( nrOfTokens / 1000 ) <<"k tokens"<<std::endl;
		pa.next();
		if( pa.isGood() ) {
		    std::list< PatternApplier >::iterator it = list_.begin();
		    ++it; // skip first elt
		    for( ; ( it != list_.end() ) && ( *it < pa ); ++it ) {
			// do nothing else
		    }
		    list_.splice( it, list_, list_.begin() );
		}
		else {
		    list_.pop_front();
		}
	    }
	    errDic.finishConstruction();
	} catch( exceptions::badInput exc ) {
	    std::wcout<<"ErrDic Creation failed: "<<exc.what()<<std::endl; 
	}
    }

} // eon
