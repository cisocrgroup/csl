#ifndef CSL_DICTSEARCH_H
#define CSL_DICTSEARCH_H CSL_DICTSEARCH_H

#include<csl/MinDic/MinDic.h>
#include<csl/FBDic/FBDic.h>
#include<csl/MSMatch/MSMatch.h>
#include<csl/LevFilter/LevFilter.h>
#include<csl/Vaam/Vaam.h>

namespace csl {
    
    /**
     *
     *
     * Revision 333 looks a lot more complex. Then I cut it down to a much simpler structure. So look at this
     * old version if e.g. support for more different dictionaries is needed.
     */
    class DictSearch {
    public:
	typedef MinDic<> HistoricDict_t;
	typedef FBDic<> ModernDict_t;
	typedef MinDic<> VaamDict_t;

	enum DictID { UNDEF, MODERN, HISTORIC };



	class Interpretation : public csl::Interpretation {
	public:
	    Interpretation() : dictID_( UNDEF ) {} 
	    
	    Interpretation( csl::Interpretation const& interpretation, DictID id ) : 
		csl::Interpretation( interpretation ),
		dictID_( UNDEF )
		{} 

	    void setDictID( DictID dictID ) { dictID_ = dictID; }
	    DictID getDictID() const { return dictID_; }
	private:
	     DictID dictID_;
	}; // class Interpretation



	class CandidateSet : public csl::LevFilter::CandidateReceiver,
			     public csl::Vaam< VaamDict_t >::iCandidateReceiver,
			     public std::vector< csl::DictSearch::Interpretation > {
	public:
	    void receive( const wchar_t* str, int levDistance, int annotation ) {
		csl::Interpretation cslInt;
		cslInt.word = str;
		cslInt.setBaseWord( str );
		cslInt.baseWordScore = annotation;
		cslInt.levDistance = levDistance;
		receive( cslInt );
	    }
	    
	    virtual void receive( csl::Interpretation const& vaam_interpretation ) {
		push_back( Interpretation( vaam_interpretation, currentDictID_ ) );
		back().setDictID( currentDictID_ );
	    }

	    void setCurrentDictID( DictID id ) { currentDictID_ = id; }

	    void reset() { clear(); }

	private:
	    DictID currentDictID_;
	}; // class CandidateSet


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

	void setModernDict( char const* dictFile, size_t dlev );
	void setModernDict( ModernDict_t const&, size_t dlev );

	void initHypothetic( char const* patternFile, size_t dlev );

	void setHistoricDict( char const* dictFile, size_t dlev );
	void setHistoricDict( HistoricDict_t const& dictFile, size_t dlev );

	//@}

	/**
	 * @name Lookup
	 */
	//@{
	void query( std::wstring const& query, CandidateSet& answers );
	//@}

    private:
	ModernDict_t const* modernDict_;
	bool disposeModernDict_;
	size_t dlev_modern_;

	Vaam< VaamDict_t >* vaam_;
	
	MSMatch< FW_BW > msMatch_;

	HistoricDict_t const* historicDict_;
	bool disposeHistoricDict_;
	size_t dlev_historic_;


    }; // class DictSearch


} // namespace csl

#endif
