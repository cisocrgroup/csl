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
	    Dict_t const* dict_;
	    bool disposeDict_;
	    size_t dLevWordlengths_[4];
	public:
	    ConfigLookup() :
		dict_( 0 ),
		disposeDict_( false )
		{
		    setDLev( 0 );
		}

	    void setDict( Dict_t const& dict ) {
		dict_ = &dict;
		disposeDict_ = false;
	    }

	    void setDLev( size_t dLev ) {
		if( dLev > 3 ) throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevStatic: choose a threshold in the range [0..3]" );
		// set minWordLengths to 0 for all levDistances smaller or equal to dLev
		for( size_t i = 0; i <= 3; ++i ) {
		    dLevWordlengths_[i] = ( i <= dLev )? 0 : INFINITE;
		}
	    }

	    void setDLevWordlengths( size_t wl0, size_t wl1 = DictSearch::INFINITE, size_t wl2 = DictSearch::INFINITE, size_t wl3 = DictSearch::INFINITE ) {
		if( wl1 < wl0 || wl2 < wl1 || wl3 < wl2 ) throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevWordlengths: minimal wordlength for distance i must be greater than for distance i-1" );

		dLevWordlengths_[0] = wl0;
		dLevWordlengths_[1] = wl1;
		dLevWordlengths_[2] = wl2;
		dLevWordlengths_[3] = wl3;
	    }
	}; // class ConfigLookup

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

	/**
	 * @brief set a modern dictionary and the levenshtein distance threshold for approximate lookup in it.
	 *
	 * @param dictFile a path to a binary file storing a csl::FBDic (produced wit hthe tool compileFBDic )
	 * @param dlev a levenshtein distance threshold 0 \le dlev \le 3
	 */
	void setModern( char const* fbDictFile, size_t dlev );
	/**
	 * @brief set a modern dictionary and the levenshtein distance threshold for approximate lookup in it.
	 *
	 * @param dictFile a path to a binary file storing a csl::FBDic (produced with the tool compileFBDic )
	 * @param dlev a levenshtein distance threshold between 0 and 3
	 *
	 * DictSearch keeps only a reference of the dictionary, so make sure that dict lives as long as you need it for DictSearch
	 */
	void setModern( Dict_t const& dict, size_t dlev );

	/**
	 * @brief set a levenshtein distance threshold for the modern dictionary approximate lookup
	 */
	void setModernDlev( size_t dlev );

	/**
	 * @brief determine the behaviour of the hypothetic dictionary.
	 * @param patternFile 
	 *
	 * You can change the levenshtein threshold later. However, at the moment you can NOT change the pattern set afterwards.
	 * Please notify the developers if you need such a feature.
	 */
	void initHypothetic( char const* patternFile, size_t dlev );

	/**
	 * @brief set a threshold for the number of pattern applications in the hypothetic dictionary
	 */
	void setHypotheticMaxNrOfPatterns( size_t max );

	/**
	 * @brief set a levenshtein distance threshold for the hypothetic dictionary approximate lookup
	 */
	void setHypotheticDlev( size_t dlev );

	
	/**
	 * @brief set a historical dictionary and the levenshtein distance threshold for approximate lookup in it.
	 *
	 * @param dictFile a path to a binary file storing a csl::FBDic (produced wit hthe tool compileFBDic )
	 * @param dlev a levenshtein distance threshold 0 \le dlev \le 3
	 */
	void setHistoric( char const* dictFile, size_t dlev );

	/**
	 * @brief set a historical dictionary and the levenshtein distance threshold for approximate lookup in it.
	 *
	 * @param dictFile a path to a binary file storing a csl::FBDic (produced with the tool compileFBDic )
	 * @param dlev a levenshtein distance threshold between 0 and 3
	 *
	 * DictSearch keeps only a reference of the dictionary, so make sure that dict lives as long as you need it for DictSearch
	 */
	void setHistoric( Dict_t const& dictFile, size_t dlev );

	/**
	 * @brief set a levenshtein distance threshold for the historic dictionary approximate lookup
	 */
	void setHistoricDlev( size_t dlev );

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
	Dict_t const* modernDict_;
	bool disposeModernDict_;
	size_t dlev_modern_;

	ConfigLookup configModern_;

	
	Vaam< VaamDict_t >* vaam_;
	size_t dlev_hypothetic_;
	size_t dlev_maxNrOfPatterns_;

	MSMatch< FW_BW > msMatch_;

	Dict_t const* historicDict_;
	bool disposeHistoricDict_;
	size_t dlev_historic_;

	class ConfigHistoric {
	    Dict_t const* dict_;
	    bool disposeDict_;
	    size_t dlev_;
	};


    }; // class DictSearch


} // namespace csl

namespace std {
    inline wostream& operator<<( wostream& os, csl::DictSearch::Interpretation const& obj ) {
	obj.print( os );
	return os;
    }
}


#endif
