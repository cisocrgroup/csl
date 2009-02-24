#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	vaam_( 0 )
    {
    }


    DictSearch::~DictSearch() {
	for( std::vector< iDictModule* >::iterator dm = dictModules_.begin(); dm != dictModules_.end(); ++dm ) {
	    delete( *dm );
	}
    }



    void DictSearch::initHypothetic( char const* patternFile ) {
	if ( vaam_ ) throw exceptions::LogicalError( "csl::DictSearch: Tried to initialise Vaam twice." );
	vaam_ = new Vaam< VaamDict_t >( dummyDic, patternFile );

	vaam_->setDistance( 0 );
	// exclude all output that is provided by levenshtein matcher on modern dict by setting a minNrOfPatterns
	vaam_->setMinNrOfPatterns( 1 );
    }


    DictSearch::DictModule& DictSearch::addDictModule( std::wstring name, std::string const& dicFile ) {
	DictModule* newDM = new DictModule( *this, name, dicFile );
	dictModules_.push_back( newDM );
	return *newDM;
    }

    DictSearch::DictModule& DictSearch::addDictModule( std::wstring name, Dict_t const& dicFile ) {
	DictModule* newDM = new DictModule( *this, name, dicFile );
	dictModules_.push_back( newDM );
	return *newDM;
    }

    void DictSearch::query( std::wstring const& query, CandidateSet* answers ) {
	for( std::vector< iDictModule* >::iterator dm = dictModules_.begin(); dm != dictModules_.end(); ++dm ) {
	    (**dm).query( query, answers );
	}
    }


    

    
} // namespace csl


#endif
