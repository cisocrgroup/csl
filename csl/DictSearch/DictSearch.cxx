#ifndef CSL_DICTSEARCH_CXX
#define CSL_DICTSEARCH_CXX CSL_DICTSEARCH_CXX

namespace csl {

    DictSearch::DictID DictSearch::addDict_exact( char const* dictFile );


    DictSearch::DictID DictSearch::addDict_exact( MinDic_t const& dic ) {
	dicts_exact_.push_back( dic );
    }

    DictSearch::DictID DictSearch::addDict_lev( char const* dictFile, size_t dlev ) {
    }

    DictSearch::DictID DictSearch::addDict_lev( MinDic_t const& dic, size_t dlev ) {
    }


    DictSearch::DictID DictSearch::addDict_vaam( char const* dictFile, char const* patternFile, size_t dlev );


    DictSearch::DictID DictSearch::addDict_vaam( MinDic_t const& dic, char const* patternFile, size_t dlev );
    



} // namespace csl

#endif
