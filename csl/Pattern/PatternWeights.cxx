#include "./PatternWeights.h"

namespace csl {
    
    PatternWeights::PatternWeights() {
    }

    void PatternWeights::reset() {
	patternWeights_.clear();
	defaultWeights_.clear();
    }

    float PatternWeights::getWeight( const csl::Pattern& pattern ) const {
	std::map< csl::Pattern, float >::const_iterator it = patternWeights_.find( pattern );
	if( it != patternWeights_.end() )
	    return it->second;
	
	return getDefault( PatternType( pattern.getLeft().size(), pattern.getRight().size() ) );
    }

    void PatternWeights::setWeight( const csl::Pattern& pattern, float weight ) {
	if( weight == UNDEF ) {
	    patternWeights_.erase( pattern );
	}
	else patternWeights_[ pattern ] = weight;
    }

    float PatternWeights::getDefault( PatternType const& patternType ) const {
	// look for a default setting suitable for the pattern's type (that is, for its ratio lefthand-size/righthand-size)
	std::map< PatternType, float >::const_iterator defaultIt = defaultWeights_.find( patternType );
	if( ( defaultIt != defaultWeights_.end() ) ) {
	    return defaultIt->second;
	}

	return UNDEF;
    }

    void PatternWeights::setDefault( PatternType const& patternType, float weight ) {
	if( weight == UNDEF ) {
	    defaultWeights_.erase( patternType );
	}
	else defaultWeights_[ patternType ] = weight;
    }

    
    void PatternWeights::printPatternWeights( std::wostream& str ) const{
	for(std::map< csl::Pattern, float >::const_iterator it = patternWeights_.begin(); it != patternWeights_.end(); it++){
	    str << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
	}
	str << "Default settings:" << std::endl;
	for(std::map< PatternType, float >::const_iterator defaultIt = defaultWeights_.begin(); defaultIt != defaultWeights_.end(); defaultIt++){
	    str << "<" <<  defaultIt->first.first << ',' << defaultIt->first.second << "> : " << defaultIt->second << std::endl;
	}
    }

}
