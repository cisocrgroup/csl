#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictSearch() :
	modernDict_( 0 ),
	disposeModernDict_( false ),
	historicDict_( 0 ),
	disposeHistoricDict_( false ),
	vaam_( 0 ),
    {
    
    }

    DictSearch::~DictSearch() {
	if( modernDict_ && disposeModernDict_ ) delete( modernDict_ );
	if( historicDict_ && disposeHistoricDict_ ) delete( historicDict_ );
    }

    void DictSearch::setModernDict( char const* dictFile ) {
	modernDict_ = new ModernDict_t( dictFile );
	disposeModernDict_ = true;
    }

    void DictSearch::setModernDict( ModernDict_t const& dic ) {
	modernDict_ = &dic;
	disposeModernDict_ = false;
    }

    void DictSearch::setHistoricDict( char const* dictFile ) {
	modernDict_ = new HistoricDict_t( dictFile );
	disposeHistoricDict_ = true;
    }

    void DictSearch::setHistoricDict( HistoricDict_t const& dic ) {
	modernDict_ = &dic;
	disposeHistoricDict_ = false;
    }

    void DictSearch::initHypothetic( char const* patternFile, size_t dlev ) {
	if( vaam_ ) throw exceptions::LogicalError( "csl::DictSearch: Tried to initialise Vaam twice." );
	vaam_ = new Vaam< ModernDict_t >( patternFile, dlev );
    }

    void query( std::wstring const& query, CandidateReceiver& answers ) {
	
    }

//     DictSearch::DictID DictSearch::addDict_lev( char const* dictFile, size_t dlev ) {
// 	return 0;
//     }

//     DictSearch::DictID DictSearch::addDict_lev( MinDic_t const& dic, size_t dlev ) {
// 	return 0;
//     }


//     DictSearch::DictID DictSearch::addDict_vaam( char const* dictFile, char const* patternFile, size_t dlev ) {
// 	return 0;
//     }


//     DictSearch::DictID DictSearch::addDict_vaam( MinDic_t const& dic, char const* patternFile, size_t dlev ) {
// 	return 0;
//     }
    



} // namespace csl

#endif
