#include "./PatternWeights.h"

namespace csl {
    
    PatternWeights::PatternWeights() : smartMerge_( false ) {
    }
    
    void PatternWeights::reset() {
	patternWeights_.clear();
	defaultWeights_.clear();
    }

    float PatternWeights::getWeight( const csl::Pattern& pattern ) const {
	std::map< csl::Pattern, float >::const_iterator it = patternWeights_.find( pattern );
	if( it != patternWeights_.end() )
	    return it->second;

	if( smartMerge_ ) {
	    if( pattern.getLeft().size() > 0 && ( pattern.getRight().find( pattern.getLeft() ) != std::wstring::npos )  ) return UNDEF;
	    if( pattern.getRight().size() > 0 && ( pattern.getLeft().find( pattern.getRight() ) != std::wstring::npos )  ) return UNDEF;
	}
	
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


    void PatternWeights::setSmartMerge( bool t ) {
	smartMerge_ = t;
    }
    

    void PatternWeights::printPatternWeights( std::wostream& str ) const{
	str << "print function is deactivated! " << std::endl;
// 	std::vector< std::pair< csl::Pattern, float > > histPatternCountSorted;
// 	for(std::map< csl::Pattern, float >::const_iterator it = histPatternCount_.begin(); it != histPatternCount_.end(); ++it ) {
// 	    histPatternCountSorted.push_back( *it );
// 	    //outStream_ << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
// 	}
// 	std::sort( histPatternCountSorted.begin(), histPatternCountSorted.end(), sortBySecond );
// 	for( std::vector< std::pair< csl::Pattern, float > >::const_iterator it = histPatternCountSorted.begin();
// 	     it != histPatternCountSorted.end();
// 	     ++it ) {
// 	    outStream_ << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
// 	    if( it - histPatternCountSorted.begin() > 30 )break;
// 	}
	
// 	for(std::map< csl::Pattern, float >::const_iterator it = patternWeights_.begin(); it != patternWeights_.end(); it++){
// 	    str << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
// 	}
// 	str << "Default settings:" << std::endl;
// 	for(std::map< PatternType, float >::const_iterator defaultIt = defaultWeights_.begin(); defaultIt != defaultWeights_.end(); defaultIt++){
// 	    str << "<" <<  defaultIt->first.first << ',' << defaultIt->first.second << "> : " << defaultIt->second << std::endl;
// 	}
//     }
    }

}
