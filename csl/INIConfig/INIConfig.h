#ifndef CSL_INICONFIG_H
#define CSL_INICONFIG_H CSL_INICONFIG_H

#include<string>
#include<iostream>
#include<fstream>
#include<cstdlib>

#include<csl/Global.h>

 
#include"./INIParser.h"


namespace csl {

    /**
     * @brief {@link INIConfig} is an object-oriented interface to the c-style "iniparser" to 
     *        parse simple ini-style configuration files.
     *
     * @author Thorsten Vobl
     * @year 2010
     */
    class INIConfig {
    public:
	INIConfig();
	~INIConfig();
	/**
	 * @todo *IMPORTANT* clarify locale behaviour!! 
	 */
	INIConfig( std::string const& iniFile );


	void load( std::string const& iniFile );


	/**
	 * @brief Finds out if a given entry exists in the configuration
	 */
	bool hasKey( std::string const& key ) const;

	char const* getstring( char const* key ) const;
	char const* getstring( std::string const& key ) const;
	int getint( char const* key ) const;
	int getint( std::string const& key ) const;
	double getdouble( char const* key ) const;
	double getdouble( std::string const& key ) const;
	/* ... */

    private:
	dictionary* dict;
    };

}

#endif
