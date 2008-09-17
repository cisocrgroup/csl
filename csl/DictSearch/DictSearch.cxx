#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

#include "./DictSearch.h"
namespace csl {

    DictSearch::DictID DictSearch::DictSearch() :
	lastDictID_( 0 ) {
    }

    DictSearch::DictID DictSearch::addDict_exact( char const* dictFile ) {
	return 0;
    }


    DictSearch::DictID DictSearch::addDict_exact( MinDic_t const& dic ) {
	DictID id = ++lastDictID_;
	dicts_exact_.push_back( ItemExact( dic, false, id ) );
	return id;
    }

    DictSearch::DictID DictSearch::addDict_lev( char const* dictFile, size_t dlev ) {
	return 0;
    }

    DictSearch::DictID DictSearch::addDict_lev( MinDic_t const& dic, size_t dlev ) {
	return 0;
    }


    DictSearch::DictID DictSearch::addDict_vaam( char const* dictFile, char const* patternFile, size_t dlev ) {
	return 0;
    }


    DictSearch::DictID DictSearch::addDict_vaam( MinDic_t const& dic, char const* patternFile, size_t dlev ) {
	return 0;
    }
    



} // namespace csl

#endif
