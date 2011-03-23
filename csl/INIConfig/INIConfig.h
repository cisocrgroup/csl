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

	/**
	 * @brief load a configuration file 
	 */
	void load( std::string const& iniFile );


	/**
	 * @brief Dumps the current configuration as a loadable ini file
	 *
	 * Note that this method is not implemented in terms of its c counterpart.
	 * Because we want to dump to a c++ filehandle
	 */
	void dump_ini( std::wostream& os ) const;

	/**
	 * @brief Finds out if a given entry exists in the configuration
	 */
	bool hasKey( std::string const& key ) const;

	/**
	 * throws a cslException if there is no such key
	 */
	char const* getstring( char const* key ) const;

	/**
	 * throws a cslException if there is no such key
	 */
	char const* getstring( std::string const& key ) const;

	/**
	 * throws a cslException if there is no such key
	 */
	int getint( char const* key ) const;

	/**
	 * throws a cslException if there is no such key
	 */
	int getint( std::string const& key ) const;

	/**
	 * throws a cslException if there is no such key
	 */
	double getdouble( char const* key ) const;

	/**
	 * throws a cslException if there is no such key
	 */
	double getdouble( std::string const& key ) const;



	/**
	 * @brief Iterates over all sections 
	 */
	class SectionIterator {
	public:
	    SectionIterator( INIConfig const& myINIConfig ) :
		myINIConfig_( myINIConfig ),
		index_( 0 ) {
		
	    }

	    inline bool operator==( SectionIterator const& other ) const {
		return ( index_ == other.index_ );
	    }

	    inline bool operator!=( SectionIterator const& other ) const {
		return ! ( index_ == other.index_ );
	    }

	    inline char const* operator*() {
		return iniparser_getsecname( myINIConfig_.dict_ , index_ );
	    }

	    inline SectionIterator& operator++() {
		++index_;
                return *this;
	    }

	private:
	    friend class INIConfig;
	    SectionIterator( INIConfig const& myINIConfig, size_t index ) :
		myINIConfig_( myINIConfig ),
		index_( index ) {
	    }

	    
	    INIConfig const& myINIConfig_;
	    size_t index_;
	};

	SectionIterator sectionsBegin() const {
	    return SectionIterator( *this );
	}

	SectionIterator sectionsEnd() const {
	    return SectionIterator( *this, iniparser_getnsec( dict_ ) );
	}


    private:
	dictionary* dict_;
    };

}

#endif
