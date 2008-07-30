
#include<string>
#include<errno.h>
#include"../MinDic/MinDic.h"



namespace csl {
    
    namespace SimpleFSA {

	class TrivialDecomp {
	public:
	    TrivialDecomp( char const* lexFile ) {
		
		computeMinDic( lexFile );
	    }

	    bool decomp( std::wstring const& word ) const {
//		std::wcout<<"!"<<word<<std::endl;
		State const* st = minDic_.getRoot();
		std::wstring::const_iterator c = word.begin();
		while( ( c != word.end() ) && 
		       ( st = st->delta( *c ) ) ) {
		    if( st->delta('#') ) {
			bool ret = decomp( word.substr( c - word.begin() + 1 ) );
			if( ret ) return true;
		    }
		    ++c;
		}
		return ( st && st->delta('$') );
	    }


	private:
	    MinDic minDic_;


	    void computeMinDic( char const* file ) {
	    std::wifstream fi( file );
	    if( ! fi.good() ) {
		throw exceptions::badFileHandle( "csl::CompoundDic: File Open Error" );
	    }
	    fi.imbue( std::locale( "de_DE.utf-8" ) );
	    minDic_.initConstruction();
	    std::wstring line;
	    size_t count = 0;
	    while( getline( fi, line).good() ) {
		if( ++count % 10000 == 0 ) std::wcerr<< count / 1000 << "k tokens processed."<<std::endl;
		if( *( line.end() - 2 ) == L'1' ) {
		    minDic_.addToken( line.substr( 0, line.size() - 3 ) + L'#' );
		}
		if( *( line.end() - 1 ) == L'1' ) {
		    minDic_.addToken( line.substr( 0, line.size() - 3 ) + L'$' );
		}
	    }
	    if( errno == EILSEQ ) {
		throw exceptions::badInput( "csl::CompoundDic: Encoding error" );
	    }

	    minDic_.finishConstruction();
	    // minDic_.toDot();

	}

	};
    }
}
