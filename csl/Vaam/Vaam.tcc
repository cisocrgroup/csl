namespace csl {

    Vaam::Vaam( const MinDic_t& baseDic, const char* patternFile ) :
	baseDic_( baseDic ),
	levDEA_( 0 ),
	maxNrOfPatterns_( 1000 )
    {
	patternGraph_.loadPatterns( patternFile );
    }

    
    void Vaam::setDistance( size_t d ) {
	levDEA_.setDistance( d );
    }

    void Vaam::setMaxNrOfPatterns( size_t n ) {
	maxNrOfPatterns_ = n;
    }

    void Vaam::query( const std::wstring& word, std::vector< Answer >* answers ) {
	query_ = word;
	answers_ = answers;
	
	levDEA_.loadPattern( query_.c_str() );
	stack_.clear();
	// create a first StackItem
	stack_.push_back( StackItem( *this ) );
	// insert an initial position at the the levDEA's start state, with 0 applied patterns
	stack_.at( 0 ).push_back( Position( levDEA_.getRoot(), 0 ) );
	// set the patternPos to the graph's root
	stack_.at( 0 ).patternPos_ = patternGraph_.getRoot();

	query_rec( 0 );
    }
    
    void Vaam::query_rec( size_t depth ) {
	
 	// std::wcout<<"query_rec( "<<depth<<" ):word="<<word_<<std::endl; // DEBUG


	stack_.push_back( StackItem( *this ) );

	// this lets us use the []-operator for stack_ with a somewhat purer conscience
	// But the at()-operator turned out to be a real bottleneck here ...
	if( stack_.size() != depth + 2 ) {
	    throw exceptions::LogicalError( "csl::Vaam::query_rec: current stack seems out of sync with backtracking procedure" );
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
			
			LevDEA::Pos newLevPos = levDEA_.walkStr( position->levPos_, rightSide->first.c_str() );
			if( newLevPos.isValid() )  {
			    //                              1 more applied pattern than cur position       store current position as 'mother'-position
			    Position newPosition( newLevPos, position->getNrOfPatternsApplied() + 1, std::make_pair( depth - patPos.getDepth(), count ) );
			    newPosition.addPosPattern( PosPattern( patternGraph_.refAt( rightSide->second ), depth - patPos.getDepth() ) );
			    stack_[depth].push_back( newPosition );
			    stack_[depth].lookAheadDepth_ = 0;
			}
		    } // for all rightSides
		} // for all positions

		patPos.walkErrorLink();
	    } while( patPos.isValid() && patPos.isFinal() && ( patPos.getDepth() >= stack_[depth].lookAheadDepth_ ) );

	} // if found left pattern side


	// report matches
	if( ( stack_[depth].lookAheadDepth_ == 0 ) && stack_[depth].dicPos_.isFinal() ) {
	    // for all positions
	    size_t count = 0;
	    for( StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position ) {
		if( levDEA_.isFinal( position->levPos_ ) ) {
		    reportMatch( &( *position ) );
		    //std::wcout<<"|d=" << levDEA_.getDistance( position->levPos_ );
		}
		++count;
	    } // for all positions
	} // report matches
	
	// for all outgoing transitions
	for( const wchar_t* c = stack_[depth].dicPos_.getSusoString();
	     *c;
	     ++c ) {
	    stack_.at( depth + 1 ).clear();

	    stack_[depth + 1].patternPos_ = stack_[depth].patternPos_.getTransTarget( *c );

	    // see which of the Positions can be moved with this *c
	    // see also if the levDEA reaches a final state
	    size_t count = 0;
	    for( StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position, ++count ) {
		LevDEA::Pos newLevPos = levDEA_.walk( position->levPos_, *c );
		if( newLevPos.isValid() ) {
		    stack_[depth + 1].push_back( Position( newLevPos, position->getNrOfPatternsApplied(), std::make_pair( depth, count ) ) );
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
		word_.resize( depth + 1 );
		word_.at( depth ) = *c;

		query_rec( depth + 1 );
	    }
	}
	
	stack_.pop_back();
    } // query_rec

    void Vaam::reportMatch( const Position* cur ) const {
	Answer answer;
	reportMatch_rec( cur, &answer );
	answer.baseWord = word_; 
	answer.word = word_; answer.instruction.applyTo( &( answer.word ) );
	answer.levDistance = levDEA_.getDistance( cur->levPos_ );

	answers_->push_back( answer );
    }

    void Vaam::reportMatch_rec( const Position* cur, Answer* answer ) const {
	if( cur->mother_.first == -1 ) {
	    return;
	}
	else {
	    reportMatch_rec( &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ), answer );
	}

	if( ! cur->posPattern_.empty() ) {
	    answer->instruction.push_back( cur->posPattern_ );
	}
    }


    
} // namespace csl
