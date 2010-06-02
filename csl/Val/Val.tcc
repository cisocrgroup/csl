namespace csl {

    Val::Val( MinDic_t const& baseDic, char const* patternFile ) :
	patternGraph_( PatternGraph::FORWARD, PatternGraph::INDEX_RIGHT ) {
	
	patternGraph_.loadPatterns( patternFile );
    }

    inline bool Val::query( std::wstring const& word, iVaamResultReceiver* interpretations ) const {

	std::vector< PatternGraph::Replacements_t const* > patternMatches( word.length(), 0 );

	
	PatternGraph::State patState = patternGraph_.getRoot();
	size_t pos = 0;
	for( std::wstring::const_iterator c = word.begin(); c != word.end(); ++c, ++pos ) {
	    patState.walk( *c );
	    if( patState.isFinal() ) {
		patternMatches.at( pos ) = &( patState.getReplacements() );
	    }
	}
    }


} // eon
