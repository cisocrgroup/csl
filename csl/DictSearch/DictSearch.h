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
	typedef MinDic<> ExactDic_t;
	typedef MinDic<> VaamDic_t;
	typedef FBDic LevDic_t;
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
			     public csl::Vaam< VaamDic_t >::iCandidateReceiver,
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


	DictSearch();

	DictID addDict_exact( char const* dictFile );
	DictID addDict_exact( ExactDic_t const& );
	DictID addDict_lev( char const* dictFile, size_t dlev );
	DictID addDict_lev( LevDic_t const&, size_t dlev );
	DictID addDict_vaam( char const* dictFile, char const* patternFile, size_t dlev );
	DictID addDict_vaam( VaamDic_t const&, char const* patternFile, size_t dlev );

	void query();

    private:
	struct ItemExact {
	    ItemExact( ExactDic_t const& dic__, bool deleteDic__, DictID id__ ) :
		dic( dic__ ),
		deleteDic( deleteDic__ ),
		id( id__ ) {
	    }
	    ExactDic_t const& dic;
	    bool deleteDic;
	    DictID id;
	};

	struct ItemLev {
	    ItemLev( MSMatch< FW_BW > const& msMatch__, bool deleteDic__, DictID id__ ) :
		dic( dic__ ),
		deleteDic( deleteDic__ ),
		id( id__ ) {
	    }
	    LevDic_t const& dic;
	    MSMatch< FW_BW > const& msMatch;
	    bool deleteDic;
	    DictID id;
	};

	struct ItemVaam {
	    Vaam_t const& vaam;
	    bool deleteDic;
	    char const* patternFile;
	    DictID id;
	};

	std::vector< ItemExact > dicts_exact_;
	std::vector< ItemLev > dicts_lev_;
	std::vector< ItemVaam > dicts_vaam_;

	DictID lastDictID_;

    }; // class DictSearch


} // namespace csl

#endif
