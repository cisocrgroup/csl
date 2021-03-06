#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	vaam_( 0 ),
	val_( 0 )
    {
    }


    DictSearch::~DictSearch() {
	// delete all DictModules that were added via addDictModule
	for( std::vector< iDictModule* >::iterator dm = internalDictModules_.begin(); dm != internalDictModules_.end(); ++dm ) {
	    delete( *dm );
	}
	if( vaam_ ) delete vaam_;
	if( val_ ) delete val_;
	
    }
    


    void DictSearch::readConfiguration( char const* configFile ) {
	INIConfig iniConf( configFile );
	readConfiguration( iniConf );
    }

    void DictSearch::readConfiguration( INIConfig const& iniConf ) {
	
	// of all sections in iniConf, find those which start with "dict_" and have a key-value pair "active = true"
	std::vector< std::string > activeDictionaries;
	for( INIConfig::SectionIterator section = iniConf.sectionsBegin(); section != iniConf.sectionsEnd(); ++section ) {
	    if( ( std::string( *section ).substr( 0, 5 ) == "dict_" ) &&
		iniConf.hasKey( std::string( *section ) + ":active" ) &&
		( strcmp( iniConf.getstring( std::string( *section ) + ":active" ), "true" ) == 0 )
		) {
		activeDictionaries.push_back( *section );
	    }
	}

	for( std::vector< std::string >::const_iterator it = activeDictionaries.begin(); it != activeDictionaries.end(); ++it ) {
	    std::wstring wideName;
	    CSLLocale::string2wstring( *it, wideName );
	    size_t cascadeRank = iniConf.hasKey( *it + ":cascadeRank" ) ? iniConf.getint( *it + ":cascadeRank" ) : 0;
	    std::wcerr << "cslDictSearch::readConfiguration: load DictModule " << wideName << ", cascadeRank=" << cascadeRank << std::endl;

	    if( ! iniConf.hasKey( *it + ":dict_type" ) ) {
		throw exceptions::cslException( std::string( "csl::DictSearch::readConfiguration: dictionary " ) 
						+ *it + " has no specified dict_type."  );
	    }

	    // Standard "word-list" dictionaries
	    if( iniConf.getstring( *it + ":dict_type" ) == std::string( "simple" ) ) {
	    
		
		DictModule& dm = addDictModule( wideName, iniConf.getstring( *it + ":path" ), cascadeRank );
		
		dm.setMaxNrOfPatterns( iniConf.getint( *it + ":histPatterns" ) );

		if( std::string( iniConf.getstring( *it + ":ocrErrors" ) ) == "LENGTH_SENSITIVE" ) {
		    dm.setDLevWordlengths();
		}
		else {
		    dm.setDLev( iniConf.getint( *it + ":ocrErrors" ) );
		}

		dm.setDLevHypothetic( iniConf.getint( *it + ":ocrErrorsOnHypothetic" ) );
		dm.setPriority( iniConf.getint( *it + ":priority" ) );
		if( iniConf.hasKey( *it + ":caseMode" ) ) {
		    if( std::string( "asIs" ) == iniConf.getstring( *it + ":caseMode" ) ) {
			dm.setCaseMode( Global::asIs );
		    }
		    else if( std::string( "toLower" ) == iniConf.getstring( *it + ":caseMode" ) ) {
			dm.setCaseMode( Global::toLower );
		    }
		    else if( std::string( "restoreCase" ) == iniConf.getstring( *it + ":caseMode" ) ) {
			dm.setCaseMode( Global::restoreCase );
		    }
		    else {
			throw exceptions::cslException( "csl::DictSearch::readConfiguration: unknown Global::caseMode" );
		    }
		}
	    }
	    // annotated dictionaries
	    else if( iniConf.getstring( *it + ":dict_type" ) == std::string( "annotated" ) ) {
		AnnotatedDictModule* sdm = new AnnotatedDictModule( *this,
									wideName, 
									iniConf.getstring( *it+ ":path" ), 
                                                			cascadeRank );
		if( std::string( iniConf.getstring( *it + ":ocrErrors" ) ) == "LENGTH_SENSITIVE" ) {
		    sdm->setDLevWordlengths();
		}
		else {
		    sdm->setDLev( iniConf.getint( *it + ":ocrErrors" ) );
		}


		if( iniConf.hasKey( *it + ":caseMode" ) ) {
		    if( std::string( "asIs" ) == iniConf.getstring( *it + ":caseMode" ) ) {
			sdm->setCaseMode( Global::asIs );
		    }
		    else if( std::string( "toLower" ) == iniConf.getstring( *it + ":caseMode" ) ) {
			sdm->setCaseMode( Global::toLower );
		    }
		    else if( std::string( "restoreCase" ) == iniConf.getstring( *it + ":caseMode" ) ) {
			sdm->setCaseMode( Global::restoreCase );
		    }
		    else {
			throw exceptions::cslException( "csl::DictSearch::readConfiguration: unknown Global::caseMode" );
		    }
		}
                
                addAnnotatedDictModule( sdm );


		
		
	    }
	    else {
		throw exceptions::cslException( "csl::DictSearch::readConfiguration: unknown dict_type for dictionary " + *it );
	    }
	    
	} // for all dictModules

	initHypothetic( iniConf.getstring( "language_model:patternFile" ) );
    }
    

    void DictSearch::initHypothetic( char const* patternFile ) {
	if ( vaam_ || val_ ) throw exceptions::LogicalError( "csl::DictSearch: Tried to initialise Vaam twice." );
	vaam_ = new Vaam< VaamDict_t >( dummyDic, patternFile );
	val_ = new Val( dummyDic, patternFile );
	
	vaam_->setDistance( 0 );
	// exclude all output that is provided by levenshtein matcher on modern dict by setting a minNrOfPatterns
	vaam_->setMinNrOfPatterns( 1 );
	val_->setMinNrOfPatterns( 1 );
    }


    DictSearch::DictModule& DictSearch::addDictModule( std::wstring const& name, std::string const& dicFile, size_t cascadeRank ) {
	DictModule* newDM = new DictModule( *this, name, dicFile, cascadeRank );
	internalDictModules_.push_back( newDM );
	allDictModules_.insert( std::make_pair( newDM->getCascadeRank(), newDM ) );
	return *newDM;
    }

    DictSearch::DictModule& DictSearch::addDictModule( std::wstring const& name, Dict_t const& dicRef, size_t cascadeRank ) {
	DictModule* newDM = new DictModule( *this, name, dicRef, cascadeRank );
	internalDictModules_.push_back( newDM );
	allDictModules_.insert( std::make_pair( newDM->getCascadeRank(), newDM ) );
	return *newDM;
    }

    DictSearch::AnnotatedDictModule& DictSearch::addAnnotatedDictModule( AnnotatedDictModule* newDM ) {
	internalDictModules_.push_back( newDM );
	allDictModules_.insert( std::make_pair( newDM->getCascadeRank(), newDM ) );
    }

    void DictSearch::addExternalDictModule( iDictModule& extModule ) {
	externalDictModules_.push_back( &extModule );
	allDictModules_.insert( std::make_pair( extModule.getCascadeRank(), &extModule ) );
    }
    

    bool DictSearch::hasDictModules() const {
	return ( internalDictModules_.size() + externalDictModules_.size() != 0 );
    }

    bool DictSearch::hasHypothetic() const {
	return ( vaam_ );
    }


    bool DictSearch::query( std::wstring const& query, iResultReceiver* answers ) {
	bool foundAnswers = false;
	
	size_t cascadeRank = 0;
	for( std::multimap< size_t, iDictModule* >::const_iterator dm = allDictModules_.begin(); 
	     dm != allDictModules_.end();
	     ++dm ) {
	    if( ((*dm).first) > cascadeRank ) {
		if( foundAnswers ) {
		    return true;
		}
		else {
		    cascadeRank = ((*dm).first);
		}
	    }
	    
//	    try {
		answers->setCurrentDictModule( *( (*dm).second ) );
		foundAnswers = ( (* (*dm).second ) ).query( query, answers );
// 	    } catch( std::exception& exc ) {
// 		std::wcerr << "csl::DictSearch::query: caught exception: " << exc.what() << std::endl;
// 	    }
	}
	return foundAnswers;

    }
    
} // namespace csl


#endif
