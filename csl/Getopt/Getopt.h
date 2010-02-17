#ifndef CSL_GETOPT_H
#define CSL_GETOPT_H CSL_GETOPT_H

#include<iostream>
#include<map>
#include<vector>
#include<stdexcept>

/**
 * @brief A class for thet parsing of command line arguments.
 * Heavily under construction!!!
 */
class Getopt {
public:
    enum Restrictiveness { IGNORE, WARN, THROW };
    enum ValueType { VOID, STRING };

    Getopt( Restrictiveness restrictiveness = IGNORE ) :
	restrictiveness_( restrictiveness ) {
    }
    
    /**
     * @brief This constructor also carries out the command line parsing. No detailed specification of flags is possible here.
     * 
     * This is mainly for backwards compatibility.
     *
     */
    Getopt( size_t argc, char const** argv ) {
	
	progName_ = argv[0];

	std::string openFlag;
	for( size_t i = 1; i < argc; ++i ) {
	    std::string word = argv[i];

	    if( word.length() > 2 && word.at( 0 ) == '-' && word.at( 1 ) == '-' ) {
		if( ! openFlag.empty() ) {
		    optionValues_[openFlag] = "1";
		    openFlag.clear();
		}

		size_t pos = word.find( '=' );
		if( pos != std::string::npos ) {
		    optionValues_[ word.substr( 2, pos - 2 ) ] = word.substr( pos + 1 );
		}
		else {
		    openFlag = word.substr( 2 );
		}
	    }
	    else {
		if( openFlag.empty() ) {
		    addArgument( word );
		}
		else {
		    optionValues_[openFlag] = word;
		    openFlag.clear();
		}
	    }
	    
	}

// 	std::cout<<"Options:"<<std::endl;
// 	for( std::map< std::string, std::string >::iterator it = options_.begin(); it != options_.end(); ++it ) {
// 	    std::cout<<it->first<<" = "<<it->second<<std::endl;
// 	}
// 	std::cout<<"arguments:"<<std::endl;
// 	for( std::vector< std::string >::iterator it = arguments_.begin(); it != arguments_.end(); ++it ) {
// 	    std::cout<<*it<<std::endl;
// 	}
    }


    void getOptions( size_t argc, char const** argv, Restrictiveness restrictiveness = IGNORE ) {
	
	progName_ = argv[0];

	std::string openFlag;
	for( size_t i = 1; i < argc; ++i ) {
	    std::string word = argv[i];

	    if( word.length() > 2 && word.at( 0 ) == '-' && word.at( 1 ) == '-' ) { // looks like a flag
		if( ! openFlag.empty() ) {
		    optionValues_[openFlag] = "1";
		    openFlag.clear();
		}

		size_t pos = word.find( '=' );
		if( pos != std::string::npos ) {
		    optionValues_[ word.substr( 2, pos - 2 ) ] = word.substr( pos + 1 );
		}
		else {
		    openFlag = word.substr( 2 );
		}
	    }
	    else { // is no flag
		if( ! openFlag.empty() && ( optionTypes_[openFlag] != VOID ) ) { // insert as value of open flag
		    optionValues_[openFlag] = word;
		    openFlag.clear();
		}
		else { // no flag waiting for value, add as argument
		    addArgument( word );
		}
	    }
	} // for all items in argv

	if( ! openFlag.empty() ) {
	}

// 	std::cout<<"Options:"<<std::endl;
// 	for( std::map< std::string, std::string >::iterator it = optionValues_.begin(); it != optionValues_.end(); ++it ) {
// 	    std::cout<<it->first<<" = "<<it->second<<std::endl;
// 	}
// 	std::cout<<"arguments:"<<std::endl;
// 	for( std::vector< std::string >::iterator it = arguments_.begin(); it != arguments_.end(); ++it ) {
// 	    std::cout<<*it<<std::endl;
// 	}
    } // getOptions

    void specifyOption( std::string const& key, ValueType valueType = VOID ) {
	optionTypes_[ key ] = valueType;
    }

    void setOption( std::string const& key, std::string const& value ) {
	optionValues_[key] = value;
    }

    void addArgument( std::string value ) {
	arguments_.push_back( value );
    }
    
    const std::string& getProgName() const {
	return progName_;
    }
    
    bool hasOption( const std::string& key ) {
	return ( optionValues_.find( key ) != optionValues_.end() );
    }

    const std::string& getOption( const std::string& key ) {
	if( ! hasOption( key ) ) throw std::runtime_error( "csl::Getopt::getOption: no such key defined");
	return optionValues_[key];
    }

    const std::string& getArgument( size_t idx ) const {
	return arguments_.at( idx );
    }

    const size_t getArgumentCount() const {
	return arguments_.size();
    }

private:
    std::string progName_;
    std::map< std::string, ValueType > optionTypes_;
    std::map< std::string, std::string > optionValues_;
    std::vector< std::string > arguments_;
    Restrictiveness restrictiveness_;

}; // class Getopt

#endif
