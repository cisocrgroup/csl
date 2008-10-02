#ifndef CSL_DICTSEARCH_H
#define CSL_DICTSEARCH_H CSL_DICTSEARCH_H

#include<ostream>
#include<csl/MinDic/MinDic.h>
#include<csl/FBDic/FBDic.h>
#include<csl/MSMatch/MSMatch.h>
#include<csl/LevFilter/LevFilter.h>
#include<csl/Vaam/Vaam.h>

namespace csl {
    
    /**
     *
     *
     * Revision 333 looked a lot more complex. Then I cut it down to a much simpler structure. So look at this
     * old version if e.g. support for more different dictionaries is needed.
     */
    class DictSearch {
    public:
	typedef FBDic<> Dict_t;
	typedef MinDic<> VaamDict_t;

	enum DictID { UNDEF, MODERN, HISTORIC };

	static const size_t INFINITE = (size_t)-1;


	class Interpretation : public csl::Interpretation {
	public:
	    Interpretation() : dictID_( UNDEF ) {} 
	    
	    Interpretation( csl::Interpretation const& interpretation, DictID id ) : 
		csl::Interpretation( interpretation ),
		dictID_( UNDEF )
		{} 

	    bool operator<( Interpretation const& other ) const {
		// set up a comparison bases on the sum of levenshtein or pattern edits.
		// Give the leevenshtein operations a marginally higher punishment, so that
		// if the sums are equal, the one with less lev. operations and more pattern 
		// operations will win.
		float compareSumOfOperations = 
		    ( getInstruction().size() + getLevDistance() * 1.01 ) - 
		    ( other.getInstruction().size() + other.getLevDistance() * 1.01 );

		if     ( compareSumOfOperations < 0 ) return true;
		else if( compareSumOfOperations > 0 ) return false;
		else if( dictID_ != other.getDictID() ) return ( dictID_ < other.getDictID() );
		else return ( getWord() < other.getWord() );
	    }

	    void setDictID( DictID dictID ) { dictID_ = dictID; }
	    DictID getDictID() const { return dictID_; }
	    std::wstring getDictID_string() const {
		if( getDictID() == UNDEF ) return L"undef";
		else if( getDictID() == MODERN ) return L"modern";
		else if( getDictID() == HISTORIC ) return L"historic";
		else return L"unknown";
	    }

	    void print( std::wostream& os = std::wcout ) const {
		csl::Interpretation::print( os );
		os << "(" << getDictID_string() << ")";
	    }

	private:
	    DictID dictID_;
	}; // class Interpretation
	


	class CandidateSet : public csl::LevFilter::CandidateReceiver,
			     public csl::Vaam< VaamDict_t >::iCandidateReceiver,
			     public std::vector< csl::DictSearch::Interpretation > {
	public:
	    void receive( const wchar_t* str, int levDistance, int annotation ) {
		csl::Interpretation cslInt;
		cslInt.setWord( str );
		cslInt.setBaseWord( str );
		cslInt.setBaseWordScore( annotation );
		cslInt.setLevDistance( levDistance );
		receive( cslInt );
	    }
	    
	    virtual void receive( csl::Interpretation const& vaam_interpretation ) {
		push_back( Interpretation( vaam_interpretation, currentDictID_ ) );
		back().setDictID( currentDictID_ );
	    }

	    void setCurrentDictID( DictID id ) { currentDictID_ = id; }

	    void reset() {
		clear();
	    }

	private:
	    DictID currentDictID_;
	}; // class CandidateSet


	class ConfigLookup {
	public:
	    ConfigLookup() :
		dict_( 0 ),
		disposeDict_( false )
		{
		    setDLev( 0 );
		}

	    ~ConfigLookup() {
		if( dict_ && disposeDict_ ) delete( dict_ );
	    }


	    Dict_t const* getDict() const {
		return dict_;
	    }

	    void setDict( Dict_t const& dict ) {
		dict_ = &dict;
		disposeDict_ = false;
	    }

	    void setDict( char const* dictFile ) {
		setDict( *( new Dict_t( dictFile ) ) );
		disposeDict_ = true;
	    }

	    void setDLev( size_t dLev ) {
		if( dLev > 3 ) throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevStatic: choose a threshold in the range [0..3]" );
		// set minWordLengths to 0 for all levDistances smaller or equal to dLev
		for( size_t i = 1; i <= 3; ++i ) {
		    minWordlengths_[i] = ( i <= dLev )? 0 : INFINITE;
		}
	    }

	    void setDLevWordlengths( size_t wl1, size_t wl2 = DictSearch::INFINITE, size_t wl3 = DictSearch::INFINITE ) {
		if( wl2 < wl1 || wl3 < wl2 ) throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevWordlengths: minimal wordlength for distance i must be greater than for distance i-1" );

		minWordlengths_[1] = wl1;
		minWordlengths_[2] = wl2;
		minWordlengths_[3] = wl3;
	    }
	    
	    size_t getDLevByWordlength( size_t wordLength ) const {
		for( int i = 3; i >= 1; --i ) {
		    if( ( minWordlengths_[i] != INFINITE ) && ( minWordlengths_[i] <= wordLength ) ) return i;
		}
		return 0;
	    }

	private:
	    Dict_t const* dict_;
	    bool disposeDict_;
	    size_t minWordlengths_[4];

	}; // class ConfigLookup

	class ConfigHypothetic : public ConfigLookup {
	public:
	    ConfigHypothetic() :
		maxNrOfPatterns_( 1 )
		{
	    }
	    size_t getMaxNrOfPatterns() const {
		return maxNrOfPatterns_;
	    }
	    void setMaxNrOfPatterns( size_t max ) {
		maxNrOfPatterns_ = max;
	    }
	private:
	    size_t maxNrOfPatterns_;
	}; 


	/**
	 * @name Constructor/ Destructor
	 */
	//@{
	DictSearch();
	~DictSearch();
	//@}

	/**
	 * @name Configuration
	 */
	//@{


	ConfigLookup& getConfigModern() {
	    return configModern_;
	}

	ConfigLookup& getConfigHistoric() {
	    return configHistoric_;
	}

	ConfigHypothetic& getConfigHypothetic() {
	    return configHypothetic_;
	}

	/**
	 * @brief initialise the hypothetic dictionary.
	 * @param patternFile A file containing a list of rewrite patterns. Consult csl::PatternSet for details
	 *
	 * At the moment you can NOT change the pattern set afterwards.
	 * Please notify the developers if you need such a feature.
	 */
	void initHypothetic( char const* patternFile );

	

	//@} // END Configuration methods

	/**
	 * @name Lookup
	 */
	//@{

	/**
	 *
	 */
	void query( std::wstring const& query, CandidateSet* answers );
	
	//@} // END Lookup methods

    private:

	ConfigLookup configModern_;
	ConfigLookup configHistoric_;
	ConfigHypothetic configHypothetic_;
	
	Vaam< VaamDict_t >* vaam_;
	size_t dlev_hypothetic_;
	size_t dlev_maxNrOfPatterns_;

	MSMatch< FW_BW > msMatch_;

    }; // class DictSearch


} // namespace csl

namespace std {
    inline wostream& operator<<( wostream& os, csl::DictSearch::Interpretation const& obj ) {
	obj.print( os );
	return os;
    }
}


#endif
