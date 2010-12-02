#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	vaam_( 0 )
    {
    }


    DictSearch::~DictSearch() {
	for( std::vector< DictModule* >::iterator dm = dictModules_.begin(); dm != dictModules_.end(); ++dm ) {
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


    DictSearch::DictModule& DictSearch::addDictModule( std::wstring const& name, std::string const& dicFile ) {
	DictModule* newDM = new DictModule( *this, name, dicFile );
	dictModules_.push_back( newDM );
	allDictModules_.insert( std::make_pair( newDM->getCascadeRank(), newDM ) );
	return *newDM;
    }

    DictSearch::DictModule& DictSearch::addDictModule( std::wstring const& name, Dict_t const& dicRef ) {
	DictModule* newDM = new DictModule( *this, name, dicRef );
	dictModules_.push_back( newDM );
	allDictModules_.insert( std::make_pair( newDM->getCascadeRank(), newDM ) );
	return *newDM;
    }

    void DictSearch::addExternalDictModule( iDictModule& extModule ) {
	externalDictModules_.push_back( &extModule );
	allDictModules_.insert( std::make_pair( extModule.getCascadeRank(), &extModule ) );
    }
    

    bool DictSearch::hasDictModules() const {
	return ( dictModules_.size() + externalDictModules_.size() != 0 );
    }


    void DictSearch::query( std::wstring const& query, iResultReceiver* answers ) {
	bool foundAnswers = false;
	size_t cascadeRank = 0;
	for( std::multimap< size_t, iDictModule* >::const_iterator dm = allDictModules_.begin(); 
	     dm != allDictModules_.end();
	     ++dm ) {
	    if( (dm.first)->getCascadeRank() > cascadeRank ) {
		if( foundAnswers ) return;
		else cascadeRank = (dm.first)->getCascadeRank();
	    }

	    try {
		answers->setCurrentDictModule( **dm );
		( *( dm->second ) ).query( query, answers );
	    } catch( std::exception& exc ) {
		std::cerr << "csl::DictSearch::query: caught exception: " << exc.what() << std::endl;
	    }

	    
	}

	for( std::vector< DictModule* >::iterator dm = dictModules_.begin(); dm != dictModules_.end(); ++dm ) {
	    try {
		answers->setCurrentDictModule( **dm );
		(**dm).query( query, answers );
	    } catch( std::exception& exc ) {
		std::cerr << "csl::DictSearch::query: caught exception: " << exc.what() << std::endl;
	    }
	}
	for( std::vector< iDictModule* >::iterator dm = externalDictModules_.begin(); dm != externalDictModules_.end(); ++dm ) {
	    try {
		answers->setCurrentDictModule( **dm );
		(**dm).query( query, answers );
	    } catch( std::exception& exc ) {
		std::cerr << "csl::DictSearch::query: caught exception: " << exc.what() << std::endl;
	    }
	}
    }
    
} // namespace csl


#endif
