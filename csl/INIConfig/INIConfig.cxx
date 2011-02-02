#ifndef CSL_INICONFIG_CXX
#define CSL_INICONFIG_CXX CSL_INICONFIG_CXX

#include "./INIConfig.h"

namespace csl {

    INIConfig::INIConfig() {
	dict = NULL;
    }

    INIConfig::INIConfig( std::string const& iniFile ) {
	dict = NULL;
	load( iniFile );
    }
    
    INIConfig::~INIConfig() {
	iniparser_freedict( dict );
    }

    void INIConfig::load( std::string const& iniFile ) {
	if( dict != 0 ) {
	    throw exceptions::cslException( "OCRC::INIConfig::load: conf was already loaded." );
	}
	dict = iniparser_load( iniFile.c_str() );
	
	if( dict == 0 ) throw exceptions::cslException( "OCRC::INIConfig: Cannot parse file" );
    }

    bool INIConfig::hasKey( std::string const& key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	return iniparser_find_entry( dict, key.c_str() );
    }

    char const* INIConfig::getstring( char const* key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	char const* c = iniparser_getstring(dict, key, NULL);
	if( ! c ) {
	    throw exceptions::cslException( std::string( "OCRC::INIConfig::geststring: key " ) + key + " not found in ini file." );
	}
	return c;
    }
    
    char const* INIConfig::getstring( std::string const& key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	return getstring( key.c_str() );
    }

    int INIConfig::getint( char const* key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	return iniparser_getint(dict, key, -1);
    }

    int INIConfig::getint( std::string const& key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	return getint( key.c_str() );
    }

    double INIConfig::getdouble( char const* key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	return iniparser_getdouble(dict, key, -1.0);
    }

    double INIConfig::getdouble( std::string const& key ) const {
	if( dict == NULL ) throw exceptions::cslException( "OCRC::INIConfig: No configuration loaded" );
	return getdouble( key.c_str() );
    }

} // eon

#endif
