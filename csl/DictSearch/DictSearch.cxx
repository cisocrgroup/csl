#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	vaam_( 0 ),
	dlev_hypothetic_( 0 ),
	dlev_maxNrOfPatterns_( 0 ) {
    }


    DictSearch::~DictSearch() {}



    void DictSearch::initHypothetic( char const* patternFile ) {
	if ( vaam_ ) throw exceptions::LogicalError( "csl::DictSearch: Tried to initialise Vaam twice." );
	vaam_ = new Vaam< VaamDict_t >( configModern_.getDict()->getFWDic(), patternFile );

	vaam_->setDistance( 0 );
	// exclude all output that is provided by levenshtein matcher on modern dict by setting a minNrOfPatterns
	vaam_->setMinNrOfPatterns( 1 );
    }


    void DictSearch::query( std::wstring const& query, CandidateSet* answers ) {
	if ( configModern_.getDict() ) {
	    answers->setCurrentDictID( MODERN );
	    msMatch_.setFBDic( *( configModern_.getDict() ) );
	    msMatch_.setDistance( configModern_.getDLevByWordlength( query.length() ) );
	    msMatch_.query( query.c_str(), *answers );

	    ////////  CHECK THIS !!!!!!!!!!!!!!
	    if ( vaam_ ) {
		vaam_->setDistance( configHypothetic_.getDLevByWordlength( query.length() ) );
		vaam_->query( query, answers );
	    }
	}
	if ( configHistoric_.getDict() ) {
	    answers->setCurrentDictID( HISTORIC );
	    msMatch_.setFBDic( *( configHistoric_.getDict() ) );
	    msMatch_.setDistance( configHistoric_.getDLevByWordlength( query.length() ) );
	    msMatch_.query( query.c_str(), *answers );
	}

    }

} // namespace csl


#endif
