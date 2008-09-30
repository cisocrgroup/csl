#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	modernDict_( 0 ),
	disposeModernDict_( false ),
	historicDict_( 0 ),
	disposeHistoricDict_( false ),
	vaam_( 0 )
    {
    
    }

    DictSearch::~DictSearch() {
	if( modernDict_ && disposeModernDict_ ) delete( modernDict_ );
	if( historicDict_ && disposeHistoricDict_ ) delete( historicDict_ );
    }

    void DictSearch::setModern( char const* dictFile, size_t dlev ) {
	setModern( *( new Dict_t( dictFile ) ), dlev );
	disposeModernDict_ = true;
    }

    void DictSearch::setModern( Dict_t const& dic, size_t dlev  ) {
	modernDict_ = &dic;
	disposeModernDict_ = false;
	setModernDlev( dlev );
    }
    
    void DictSearch::setModernDlev( size_t dlev ) {
	dlev_modern_ = dlev;
    }

    void DictSearch::setHistoric( char const* dictFile, size_t dlev  ) {
	historicDict_ = new Dict_t( dictFile );
	disposeHistoricDict_ = true;
    }

    void DictSearch::setHistoric( Dict_t const& dic, size_t dlev  ) {
	historicDict_ = &dic;
	disposeHistoricDict_ = false;
    }
    
    void DictSearch::setHistoricDlev( size_t dlev ) {
	dlev_modern_ = dlev;
    }

    void DictSearch::initHypothetic( char const* patternFile, size_t dlev ) {
	if( vaam_ ) throw exceptions::LogicalError( "csl::DictSearch: Tried to initialise Vaam twice." );
	vaam_ = new Vaam< VaamDict_t >( modernDict_->getFWDic(), patternFile );
	
	vaam_->setDistance( dlev );
	// exclude all output that is provided by levenshtein matcher on modern dict
	vaam_->setMinNrOfPatterns( 1 );
    }

    void DictSearch::query( std::wstring const& query, CandidateSet& answers ) {
	if( modernDict_ ) {
	    msMatch_.setFBDic( *modernDict_ );
	    msMatch_.setDistance( dlev_modern_ );
	    msMatch_.query( query.c_str(), answers );
	}
    }

} // namespace csl

#endif
