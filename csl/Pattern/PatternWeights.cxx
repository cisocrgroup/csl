#include "./PatternWeights.h"

namespace csl {
    
    PatternWeights::PatternWeights() {
    }

    float PatternWeights::getWeight( const csl::Pattern& pattern ) const {
	std::map< csl::Pattern, float >::const_iterator it = patternWeights_.find( pattern );
	if( it != patternWeights_.end() )
	    return it->second;
	
	return getDefault( std::make_pair( pattern.getLeft().size(), pattern.getRight().size() ) );
    }

    void PatternWeights::setWeight( const csl::Pattern& pattern, float weight ) {
	patternWeights_[ pattern ] = weight;
    }

    float PatternWeights::getDefault( std::pair< size_t, size_t > const& patternType ) const {
	// look for a default setting suitable for the pattern's type (that is, for its ratio lefthand-size/righthand-size)
	std::map< PatternType, float >::const_iterator defaultIt = defaultWeights_.find( std::make_pair( patternType.first, patternType.second ) );
	if( defaultIt != defaultWeights_.end() )
	    return defaultIt->second;
	
	return UNDEF;
    }

    void PatternWeights::setDefault( std::pair< size_t, size_t > const& patternType, float weight ) {
	defaultWeights_[ patternType ] = weight;
    }

    
    void PatternWeights::printPatternWeights() const{
	for(std::map< csl::Pattern, float >::const_iterator it = patternWeights_.begin(); it != patternWeights_.end(); it++){
	    std::wcout << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
	}
	std::wcout << "Default settings:" << std::endl;
	for(std::map< PatternType, float >::const_iterator defaultIt = defaultWeights_.begin(); defaultIt != defaultWeights_.end(); defaultIt++){
	    std::wcout << "<" <<  defaultIt->first.first << ',' << defaultIt->first.second << " : " << defaultIt->second << std::endl;
	}
    }

}
