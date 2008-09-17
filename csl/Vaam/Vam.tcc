namespace csl {

    bool hasSubstring( std::wstring const& str, size_t pos, std::wstring const& substr ); // forward declaration

    Vam::Vam( const MinDic_t& baseDic, const char* patternFile ) :
	baseDic_( baseDic ),
	filterDic_( 0 ),
	levDEA_( 0 ), // Here the default levenshtein threshold is specified
	minNrOfPatterns_( 0 ),
	maxNrOfPatterns_( 1000 )
    {
	patternGraph_.loadPatterns( patternFile );
    }

    
    void Vam::setDistance( size_t d ) {
	levDEA_.setDistance( d );
    }

    void Vam::setMinNrOfPatterns( size_t n ) {
	minNrOfPatterns_ = n;
    }

    void Vam::setMaxNrOfPatterns( size_t n ) {
	maxNrOfPatterns_ = n;
    }

    bool Vam::query( const std::wstring& word, std::vector< Interpretation >* interpretations ) const {
	query_ = word;
	interpretations_ = interpretations;
	interpretations_->clear();
	levDEA_.loadPattern( query_.c_str() );
	stack_.clear();
	// create a first StackItem
	stack_.push_back( StackItem( *this ) );
	// insert an initial position at wordPos 0, with 0 applied patterns
	stack_.at( 0 ).push_back( Position( 0, 0 ) );
	// set the patternPos to the graph's root
	stack_.at( 0 ).patternPos_ = patternGraph_.getRoot();

	query_rec( 0 );

	return ! interpretations_->empty();
    }
    
    void Vam::setFilterDic( MinDic_t const& filterDic ) {
	filterDic_ = &filterDic;
    }


    void Vam::query_rec( size_t depth ) const {
	
 	// std::wcout<<"query_rec( "<<depth<<" ):word="<<baseWord_<<std::endl; // DEBUG


	stack_.push_back( StackItem( *this ) );

	// this lets us use the []-operator for stack_ with a somewhat purer conscience
	// But the at()-operator turned out to be a real bottleneck here ...
	if( stack_.size() != depth + 2 ) {
	    throw exceptions::LogicalError( "csl::Vam::query_rec: current stack seems out of sync with backtracking procedure" );
	}

	// apply all patterns whose left sides end here 
	if( stack_[depth].patternPos_.isFinal() ) {
	    PatternGraph::State patPos = stack_[depth].patternPos_;
	    
	    do { // for all final states reachable via errorLinks
		assert( patPos.isFinal() );
		size_t count = 0;

		// for all positions of the stackItem (tracked back leftside)
	    	for( StackItem::iterator position = stack_.at( depth - patPos.getDepth() ).begin();
		     position != stack_.at( depth - patPos.getDepth() ).end();
		     ++position, ++count ) {
		    if( position->getNrOfPatternsApplied() == maxNrOfPatterns_ )
			continue;

		    // for all right sides fitting the current leftSide
		    // Note that there might be final states with empty rightSides, namely those where a suffix
		    // of the current path leads to a "real" final state with non-empty rightSides
		    for( PatternGraph::RightSides_t::const_iterator rightSide = patPos.getRightSides().begin();
			 rightSide != patPos.getRightSides().end();
			 ++rightSide ) {
			
			if( hasSubstring( query_, position->wordPos_, rightSide->first ) )  {
			    //                              1 more applied pattern than cur position       store current position as 'mother'-position
			    Position newPosition( position->wordPos_ + rightSide->first.length(), position->getNrOfPatternsApplied() + 1, std::make_pair( depth - patPos.getDepth(), count ) );
			    newPosition.addPosPattern( PosPattern( patternGraph_.at( rightSide->second ).getLeft(),
								   patternGraph_.at( rightSide->second ).getRight(),
								   depth - patPos.getDepth() ) );
			    stack_[depth].push_back( newPosition );
			    stack_[depth].lookAheadDepth_ = 0;
			}
		    } // for all rightSides
		} // for all positions

		patPos.walkErrorLink();
	    } while( patPos.isValid() && patPos.isFinal() && ( patPos.getDepth() >= stack_[depth].lookAheadDepth_ ) );

	} // if found left pattern side


	// report matches
	if( ( stack_[depth].lookAheadDepth_ == 0 ) &&  // we are not in the lookahead-phase AND
	    ( stack_[depth].dicPos_.isFinal() ) // arrived at a final state of the dic
	    ) {
	    // for all positions
	    size_t count = 0;
	    for( StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position ) {
		if( position->wordPos_ == query_.size() ) {
		    reportMatch( &( *position ), stack_[depth].dicPos_.getAnnotation() );
		    //std::wcout<<"|d=" << levDEA_.getDistance( position->levPos_ );
		}
		++count;
	    } // for all positions
	} // report matches
	
	// for all outgoing transitions
	for( const wchar_t* c = stack_[depth].dicPos_.getSusoString();
	     *c;
	     ++c ) {
	    stack_[depth + 1].clear();

	    stack_[depth + 1].patternPos_ = stack_[depth].patternPos_.getTransTarget( *c );

	    // see which of the Positions can be moved with this *c
	    size_t count = 0;
	    for( StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position, ++count ) {
		if( ( position->wordPos_ <  query_.length() ) && ( query_.at( position->wordPos_ ) == *c ) ) {
		    stack_[depth + 1].push_back( Position( position->wordPos_ + 1, position->getNrOfPatternsApplied(), std::make_pair( depth, count ) ) );
		}
	    }
	    
	    // set lookAheadDepth_ for next stackItem: 
	    if( ! stack_[depth + 1].empty() ) {
		// set to 0 if new positions exist
		stack_[depth + 1].lookAheadDepth_ = 0;
	    }
	    else {
		// increment if no new positions exist
		stack_[depth + 1].lookAheadDepth_ = stack_[depth].lookAheadDepth_ + 1;
	    }

	    if( stack_[depth + 1].lookAheadDepth_ <= stack_[depth + 1].patternPos_.getDepth() ) {
		stack_[depth + 1].dicPos_ = stack_[depth].dicPos_.getTransTarget( *c );
		baseWord_.resize( depth + 1 );
		baseWord_.at( depth ) = *c;

		query_rec( depth + 1 );
	    }
	}
	
	stack_.pop_back();
    } // query_rec

    void Vam::reportMatch( const Position* cur, int baseWordScore ) const {
	Interpretation interpretation;

	interpretation.setBaseWord( baseWord_ ); 
	reportMatch_rec( cur, &interpretation );
	interpretation.levDistance = 0;
	interpretation.baseWordScore = baseWordScore;

	// find out what word we're talking about by applying the pattern to the baseWord
	interpretation.word = baseWord_; interpretation.instruction.applyTo( &( interpretation.word ) );

	if( filterDic_ && filterDic_->lookup( interpretation.word ) ) { // if there's a filterDic_ and interpretation.word is in it
	    return;
	}

	if( interpretation.instruction.size() < minNrOfPatterns_ ) { // if we have less than the minimal nr of patterns
	    return;
	}

	interpretations_->push_back( interpretation );
    }

    void Vam::reportMatch_rec( const Position* cur, Interpretation* interpretation ) const {
	if( cur->mother_.first == -1 ) {
	    return;
	}
	else {
	    reportMatch_rec( &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ), interpretation );
	}

	if( ! cur->posPattern_.empty() ) {
	    interpretation->instruction.push_back( cur->posPattern_ );
	}
    }

    bool hasSubstring( std::wstring const& str, size_t pos, std::wstring const& substr ) {
	std::wcout<<"does "<<str<<" have "<<substr<<" at pos "<<pos<<std::endl;
	std::wstring::const_iterator c_str = str.begin() + pos;
	for( std::wstring::const_iterator c_sub = substr.begin(); 
	     c_sub != substr.end(); 
	     ++c_sub, ++c_str ) {
	    if( c_str == str.end() ) return false;
	    if( *c_sub != *c_str ) return false;
	}
	return true;
    }

    
} // namespace csl
