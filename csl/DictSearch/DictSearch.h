#ifndef CSL_DICTSEARCH_H
#define CSL_DICTSEARCH_H CSL_DICTSEARCH_H

#include<csl/MinDic/MinDic.h>
#include<csl/LevFilter/LevFilter.h>
#include<csl/Vaam/Vaam.h>

namespace csl {
    
    /**
     * 
     */
    class DictSearch {
    public:
	typedef MinDic<> MinDic_t;
	typedef Vaam<> Vaam_t;
	typedef size_t DictID;



	class Interpretation : public csl::Interpretation {
	public:
	    Interpretation() : dictID_( 0 ) {} 

	    Interpretation( csl::Interpretation const& interpretation, DictID id ) : 
		csl::Interpretation( interpretation ),
		dictID_( 0 )
		{} 

	    void setDictID( DictID dictID ) { dictID_ = dictID; }
	    DictID getDictID() const { return dictID_; }
	private:
	     DictID dictID_;
	}; // class Interpretation



	class CandidateSet : public csl::LevFilter::CandidateReceiver,
			     public csl::Vaam< MinDic_t >::iCandidateReceiver,
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



	DictID addDict_exact( char const* dictFile );
	DictID addDict_exact( MinDic_t const& );
	DictID addDict_lev( char const* dictFile, size_t dlev );
	DictID addDict_lev( MinDic_t const&, size_t dlev );
	DictID addDict_vaam( char const* dictFile, char const* patternFile, size_t dlev );
	DictID addDict_vaam( MinDic_t const&, char const* patternFile, size_t dlev );

	void query();

    private:
	struct ItemExact {
	    ItemExact( MinDic_t const& dic__, bool doDelete__ ) :
		dic( dic__ ),
		doDelete( doDelete__ ) {
	    }
	    MinDic_t const& dic;
	    bool doDelete;
	};
	struct ItemLev {
	    MinDic_t const& dic;
	    bool doDelete;
	};
	struct ItemVaam {
	    Vaam_t const& vaam;
	    bool doDelete;
	    char const* patternFile;
	};

	std::vector< ItemExact > dicts_exact_;
	std::vector< ItemLev > dicts_lev_;
	std::vector< ItemVaam > dicts_vaam_;

    }; // class DictSearch


} // namespace csl

#endif
