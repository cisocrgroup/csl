#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	modernDict_( 0 ),
	disposeModernDict_( false ),
	dlev_modern_( 0 ),
	vaam_( 0 ),
	dlev_hypothetic_( 0 ),
	dlev_maxNrOfPatterns_( 0 ),
	historicDict_( 0 ),
	disposeHistoricDict_( false ),
	dlev_historic_( 0 )
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
	setHistoricDlev( dlev );
    }

    
    void DictSearch::setHistoricDlev( size_t dlev ) {
	dlev_historic_ = dlev;
    }


    void DictSearch::initHypothetic( char const* patternFile, size_t dlev ) {
	if( vaam_ ) throw exceptions::LogicalError( "csl::DictSearch: Tried to initialise Vaam twice." );
	vaam_ = new Vaam< VaamDict_t >( modernDict_->getFWDic(), patternFile );
	
	vaam_->setDistance( dlev );
	// exclude all output that is provided by levenshtein matcher on modern dict
	vaam_->setMinNrOfPatterns( 1 );
    }


    void DictSearch::setHypotheticMaxNrOfPatterns( size_t max ) {
	dlev_maxNrOfPatterns_ = max;
	vaam_->setMaxNrOfPatterns( dlev_maxNrOfPatterns_ );
    }


    void DictSearch::setHypotheticDlev( size_t dlev ) {
	dlev_hypothetic_ = dlev;
	if( ! vaam_ ) throw exceptions::LogicalError( "csl::DictSearch::setHypotheticDlev: Hypothetic dic not initialised" );
	vaam_->setDistance( dlev );
    }



    void DictSearch::query( std::wstring const& query, CandidateSet* answers ) {
	if( modernDict_ ) {
	    answers->setCurrentDictID( MODERN );
	    msMatch_.setFBDic( *modernDict_ );
	    msMatch_.setDistance( dlev_modern_ );
	    msMatch_.query( query.c_str(), *answers );

	    if( vaam_ ) {
		vaam_->query( query, answers );
	    }
	}
	if( historicDict_ ) {
	    answers->setCurrentDictID( HISTORIC );
	    msMatch_.setFBDic( *historicDict_ );
	    msMatch_.setDistance( dlev_historic_ );
	    msMatch_.query( query.c_str(), *answers );
	}

    }

} // namespace csl


#endif
