#ifndef CSL_INICONFIG_CXX
#define CSL_INICONFIG_CXX CSL_INICONFIG_CXX

#include "./INIConfig.h"

namespace csl {

    INIConfig::INIConfig() {
	dict_ = NULL;
    }

    INIConfig::INIConfig( std::string const& iniFile ) {
	dict_ = NULL;
	load( iniFile );
    }
    
    INIConfig::~INIConfig() {
	iniparser_freedict( dict_ );
    }

    void INIConfig::load( std::string const& iniFile ) {
	if( dict_ != 0 ) {
	    throw exceptions::cslException( "csl::INIConfig::load: conf was already loaded." );
	}

	dict_ = iniparser_load( iniFile.c_str() );
	
	if( dict_ == 0 ) throw exceptions::cslException( "csl::INIConfig: Cannot parse file" );
    }

    void INIConfig::dump_ini( std::wostream& os ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );

	std::string sectionName;
	std::string sectionNameColon; // holds the section name plus a colon :  "someSection:"
	
	std::wstring wideSection, wideKey, wideValue; // used for various conversions

	size_t nsec = iniparser_getnsec( dict_ );
	if ( nsec < 1 ) {
	    /* No section in file: dump all keys as they are */
	    for ( int i = 0 ; i < dict_->size ; i++) {
		if (dict_->key[i]==NULL)
		    continue ;

		csl::string2wstring( dict_->key[i], wideKey );
		csl::string2wstring( dict_->val[i], wideValue );
		os << wideKey << " = " << wideValue << std::endl;
	    }
	    return ;
	}
	else { // there are sections
	    for ( int i = 0 ; i < nsec ; i++ ) {
		sectionName = iniparser_getsecname(dict_, i) ;
		csl::string2wstring( sectionName, wideSection );
		os << std::endl << '[' << wideSection << ']' << std::endl;

		sectionNameColon = sectionName + ':';
		for ( int j=0 ; j<dict_->size ; j++) {
		    if ( dict_->key[j] == NULL ) {
			continue ;
		    }
		    if( !strncmp(dict_->key[j], sectionNameColon.c_str(), sectionNameColon.size() ) ) {
			csl::string2wstring( dict_->key[j], wideKey );
			if( dict_->val[j] ) { // obviously this can be 0 
			    wideValue.clear();
			}
			else {
			    csl::string2wstring( dict_->val[j], wideValue );
			}

			os << wideKey.substr( sectionNameColon.size() ) << " = " << wideValue << std::endl;
			
		    }
		}
	    }
	    os << std::endl;
	    return ;
	}
    }
    
    
    bool INIConfig::hasKey( std::string const& key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	return iniparser_find_entry( dict_, key.c_str() );
    }

    char const* INIConfig::getstring( char const* key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	char const* c = iniparser_getstring(dict_, key, NULL);
	if( ! c ) {
	    throw exceptions::cslException( std::string( "csl::INIConfig::geststring: key " ) + key + " not found in ini file." );
	}
	return c;
    }
    
    char const* INIConfig::getstring( std::string const& key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	return getstring( key.c_str() );
    }

    int INIConfig::getint( char const* key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	return iniparser_getint(dict_, key, -1);
    }

    int INIConfig::getint( std::string const& key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	return getint( key.c_str() );
    }

    double INIConfig::getdouble( char const* key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	return iniparser_getdouble(dict_, key, -1.0);
    }

    double INIConfig::getdouble( std::string const& key ) const {
	if( dict_ == NULL ) throw exceptions::cslException( "csl::INIConfig: No configuration loaded" );
	return getdouble( key.c_str() );
    }

} // eon

#endif