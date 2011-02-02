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
	for( std::vector< DictModule* >::iterator dm = dictModules_.begin(); dm != dictModules_.end(); ++dm ) {
	    delete( *dm );
	}
	if( vaam_ ) delete vaam_;
	if( val_ ) delete val_;

    }
    


    void DictSearch::readConfiguration( char const* configFile, std::string const& prefix ) {
	INIConfig iniConf( configFile );
	readConfiguration( iniConf, prefix );
    }

    void DictSearch::readConfiguration( INIConfig const& iniConf, std::string const& prefix ) {
	
	std::vector< std::string > activeDictionaries;
	split( std::string( iniConf.getstring("dictionaries:activeDictionaries" ) ), ' ', &activeDictionaries );

	for( std::vector< std::string >::const_iterator it = activeDictionaries.begin(); it != activeDictionaries.end(); ++it ) {
	    std::wstring wideName;
	    csl::string2wstring( *it, wideName );
	    std::wcerr << "OCRC::Profiler::readConfiguration: load DictModule " << wideName << std::endl;
	    DictModule& dm = addDictModule( wideName, iniConf.getstring( *it + ":path" ), iniConf.getint( *it + ":cascadeRank" ) );


	    dm.setMaxNrOfPatterns( iniConf.getint( *it + ":histPatterns" ) );
	    dm.setDLev( iniConf.getint( *it + ":ocrErrors" ) );
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
	dictModules_.push_back( newDM );
	allDictModules_.insert( std::make_pair( newDM->getCascadeRank(), newDM ) );
	return *newDM;
    }

    DictSearch::DictModule& DictSearch::addDictModule( std::wstring const& name, Dict_t const& dicRef, size_t cascadeRank ) {
	DictModule* newDM = new DictModule( *this, name, dicRef, cascadeRank );
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

	// OLD VERSION, without cascading queries
	// for( std::vector< DictModule* >::iterator dm = dictModules_.begin(); dm != dictModules_.end(); ++dm ) {
	//     try {
	// 	answers->setCurrentDictModule( **dm );
	// 	(**dm).query( query, answers );
	//     } catch( std::exception& exc ) {
	// 	std::cerr << "csl::DictSearch::query: caught exception: " << exc.what() << std::endl;
	//     }
	// }
	// for( std::vector< iDictModule* >::iterator dm = externalDictModules_.begin(); dm != externalDictModules_.end(); ++dm ) {
	//     try {
	// 	answers->setCurrentDictModule( **dm );
	// 	(**dm).query( query, answers );
	//     } catch( std::exception& exc ) {
	// 	std::cerr << "csl::DictSearch::query: caught exception: " << exc.what() << std::endl;
	//     }
	// }
    }
    
} // namespace csl


#endif
