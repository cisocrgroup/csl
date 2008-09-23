#ifndef CSL_DICTSEARCH_H
#define CSL_DICTSEARCH_H CSL_DICTSEARCH_H

#include<csl/MinDic/MinDic.h>
#include<csl/FBDic/FBDic.h>
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
	typedef MinDic<> HistDict_t;
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
			     std::vector< csl::DictSearch::Interpretation > {
	public:
	    void receive( const wchar_t* str, int levDistance, int annotation ) {
	    }

	    virtual void receive( csl::Interpretation const& vaam_interpretation ) {
		push_back( Interpretation( vaam_interpretation, currentDictID_ ) );
		back().setDictID( currentDictID_ );
	    }

	    void setCurrentDictID( DictID id ) { currentDictID_ = id; }

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

	void setModernDict( char const* dictFile );
	void setModernDict( ModernDict_t const& );
	void setHistoricDict( char const* dictFile );
	void setHistoricDict( HistDict_t const& );
	void initHypothetic( char const* patternFile, size_t dlev );
	void setLevDistance( size_t dlev );
	//@}

	/**
	 * @name Lookup
	 */
	//@{
	void query();
	//@}

    private:
	ModernDict_t const* modernDict_;
	bool disposeModernDict_;
	HistDict_t const* historicDict_;
	bool disposeHistoricDict_;

	Vaam< ModernDict_t >* vaam_;

    }; // class DictSearch


} // namespace csl

#endif
