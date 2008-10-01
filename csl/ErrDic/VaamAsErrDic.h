#ifndef CSL_VAAM_AS_ERRDIC_H
#define CSL_VAAM_AS_ERRDIC_H CSL_VAAM_AS_ERRDIC_H

#include<csl/ErrDic/iErrDic.h>
#include<csl/Vaam/Vaam.h>

namespace csl {
    
    /**
     * @brief This class uses Vaam to implement the behaviour of an error dictionary
     *
     */
    class VaamAsErrDic :
	public iErrDic {

    public:
	VaamAsErrDic( MinDic<> const& baseDic, char const* patternFile, MinDic<> const& filterDic );

	bool query( std::wstring const& word, Interpretation* answer ) const;

	void setMinWordLength( size_t minLength );

    private:
	Vaam<> vaam_;
	size_t minWordLength_;
	mutable Vaam<>::CandidateReceiver answers_;
    };

    
    VaamAsErrDic::VaamAsErrDic( MinDic<> const& baseDic, char const* patternFile, MinDic<> const& filterDic ) :
	vaam_( baseDic, patternFile ),
	minWordLength_( 0 ) {
	
	vaam_.setDistance( 0 );
	vaam_.setMaxNrOfPatterns( 1 );
	vaam_.setFilterDic( filterDic );
    }

    bool VaamAsErrDic::query( std::wstring const& word, Interpretation* answer ) const {
//	std::wcerr<<"VaamAsErrDic::query: "<<word<<std::endl;
	answer->clear();
	answers_.clear();

	if( word.size() < minWordLength_ ) {
	    return false;
	}

	if( vaam_.query( word, &answers_ ) ) {

	    if( answers_.size() == 1 ) {
		*answer = answers_.at( 0 );
	    }
	    else { // disambiguate: choose the interpretation with the best baseWordScore
		std::vector< Interpretation >::const_iterator it = answers_.begin();
		*answer = *it;++it;
		for( ;
		     it != answers_.end();
		     ++it ) {
		    if( it->getBaseWordScore() > answer->getBaseWordScore() ) {
			// std::wcout<<"Replace: "<<answer->baseWord<<"-->"<<it->baseWord<<std::endl;
			*answer = *it;
		    }
		}
	    }

	    return true;
	}
	else return false;
    }

    void VaamAsErrDic::setMinWordLength( size_t minLength ) {
	minWordLength_ = minLength;
    }



} // eon





#endif
