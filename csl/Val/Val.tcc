namespace csl {

    Val::Val( MinDic_t const& baseDic, char const* patternFile ) :
	patternGraph_( PatternGraph::FORWARD, PatternGraph::INDEX_RIGHT ) {

	baseDic_ = &baseDic;
	patternGraph_.loadPatterns( patternFile );
    }

    inline bool Val::query( std::wstring const& word, iInterpretationReceiver* interpretations ) const {
	query_ = word;
	interpretations_ = interpretations;
	
	stack_.clear();
	stack_.reserve( word.length() + 1 );
	stack_.push_back( StackItem() );
	stack_.back().push_back( Position( baseDic_->getRootState() ) );
	
	MinDic_t::State tmp = baseDic_->getRootState();
	size_t stmp = baseDic_->getRoot();


	size_t depth = 1; // depth 0 was initialized above
	PatternGraph::State patternPos = patternGraph_.getRoot();
	for( std::wstring::const_iterator c = word.begin(); c != word.end(); ++c, ++depth ) {
	    
	    StackItem const& last = stack_.back();
	    stack_.push_back( StackItem() );
	    StackItem& cur = stack_.back();

	    std::wcout << "depth=" << depth << ",#pos=" << last.size() << ", patPos=" << patternPos.getStateIndex() << std::endl;

	    size_t count = 0;
	    for( StackItem::const_iterator pos = last.begin(); pos != last.end(); ++pos, ++count ) {
		if( pos->dicPos_.hasTransition( *c ) ) {
		    cur.push_back( 
			Position(
			    pos->dicPos_.getTransTarget( *c ),
			    pos->nrOfPatternsApplied_,
			    std::make_pair( depth - 1, count)
			    )
			);
		}
	    }
	    
	    patternPos.walk( *c );
	    if( patternPos.isFinal() ) {
		// for all patterns fitting a suffix of the current path
		for( PatternGraph::Replacements_t::const_iterator leftSide = patternPos.getReplacements().begin();
		     leftSide != patternPos.getReplacements().end();
		     ++leftSide ) {
		    
		    size_t sizeOfRightSide = patternGraph_.at( leftSide->second ).getRight().length();
		    
		    // for all positions of the stackItem (tracked back leftside)
		    size_t count = 0;
		    for( StackItem::const_iterator pos = stack_.at( depth - sizeOfRightSide ).begin();
			 pos != stack_.at( depth - sizeOfRightSide ).end();
			 ++pos, ++count ) {
			
			// check if maxNrOfPatterns_ is reached already
			if( ( maxNrOfPatterns_ != Val::INFINITE ) && ( pos->getNrOfPatternsApplied() == maxNrOfPatterns_ ) )
			    continue;
			
			MinDic_t::State newDicPos = pos->dicPos_.getTransTarget( leftSide->first.c_str() );
			if( newDicPos.isValid() )  {
			    //                              1 more applied pattern than cur position       store current position as 'mother'-position
			    Position newPosition( newDicPos, pos->getNrOfPatternsApplied() + 1, std::make_pair( depth - sizeOfRightSide, count ) );
			    newPosition.addPosPattern( PosPattern( patternGraph_.at( leftSide->second ).getLeft(),
								   patternGraph_.at( leftSide->second ).getRight(),
								   depth - sizeOfRightSide ) );
			    cur.push_back( newPosition );
			    cur.lookAheadDepth_ = 0;
			}
		    } // for all rightSides
		} // for all positions
		
	    }
	    
	    
	} // for all chars of the query word

	// report matches
	if( ( stack_.back().lookAheadDepth_ == 0 ) ) {  // we are not in the lookahead-phase AND
	    // for all positions
	    size_t count = 0;
	    for( StackItem::iterator position = stack_.back().begin();
		 position != stack_.back().end();
		 ++position ) {
		if( position->dicPos_.isFinal() ) {
		    reportMatch( &( *position ), position->dicPos_.getAnnotation() );
		}
		++count;
	    } // for all positions
	} // report matches
	
    } // method query
    
    void Val::reportMatch( const Position* cur, int baseWordScore ) const {
	Interpretation interpretation;

	reportMatch_rec( cur, &interpretation );
	interpretation.setLevDistance( 0 );
	interpretation.setBaseWordScore( baseWordScore );

	interpretation.setWord( query_ ); 
	// find out what baseWord we're talking about by applying the pattern to the query
	std::wstring word = query_;
	interpretation.getInstruction().applyTo( &word );
	interpretation.setBaseWord( word ); 

// 	if( ( filterDic_ && filterDic_->lookup( interpretation.getWord() ) )  || // if there's a filterDic_ and interpretation.word is in it or ..
// 	    ( interpretation.getInstruction().size() < minNrOfPatterns_ )
	    
// 	    ) {
// 	    return;
// 	}

 	interpretations_->receive( interpretation );
   }

    void Val::reportMatch_rec( const Position* cur, Interpretation* interpretation ) const {
	if( cur->mother_.first == -1 ) {
	    return;
	}
	else {
	    //std::wcout << cur->mother_.first << "," << cur->mother_.second << "," << stack_.at( cur->mother_.first ).size() << std::endl;
	    reportMatch_rec( &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ), interpretation );
	}

	if( ! cur->posPattern_.empty() ) {
	    interpretation->getInstruction().push_back( cur->posPattern_ );
	}
    }

    
} // eon