#ifndef CSL_GETOPT_H
#define CSL_GETOPT_H CSL_GETOPT_H

#include<iostream>
#include<map>
#include<vector>

/**
 * @brief A class for thet parsing of command line arguments.
 * Heavily under construction!!!
 */
class Getopt {
public:
    Getopt( size_t argc, const char** argv ) {
	
	progName_ = argv[0];

	std::string openFlag;
	for( size_t i = 1; i < argc; ++i ) {
	    std::string word = argv[i];

	    if( word.length() > 2 && word.at( 0 ) == '-' && word.at( 1 ) == '-' ) {
		if( ! openFlag.empty() ) {
		    options_[openFlag] = "1";
		    openFlag.clear();
		}

		size_t pos = word.find( '=' );
		if( pos != std::string::npos ) {
		    options_[ word.substr( 2, pos - 2 ) ] = word.substr( pos + 1 );
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
		    options_[openFlag] = word;
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

	void setOption( const std::string& key, const std::string& value ) {
		options_[key] = value;
    }
	void addArgument( std::string value ) {
		arguments_.push_back( value );
	}

	const std::string& getProgName() const {
	return progName_;
    }

    bool hasOption( const std::string& key ) {
	return ( options_.find( key ) != options_.end() );
    }

    const std::string& getOption( const std::string& key ) {
	return options_[key];
    }

    const std::string& getArgument( size_t idx ) const {
	return arguments_.at( idx );
    }

    const size_t getArgumentCount() const {
	return arguments_.size();
    }

private:
    std::string progName_;
    std::map< std::string, std::string > options_;
    std::vector< std::string > arguments_;

}; // class Getopt

#endif
