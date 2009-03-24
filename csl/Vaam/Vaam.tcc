#ifndef CSL_VAAM_TCC
#define CSL_VAAM_TCC CSL_VAAM_TCC

namespace csl {

    template< typename MinDicType >
    Vaam< MinDicType >::Vaam( const MinDic_t& baseDic, const char* patternFile ) :
	baseDic_( &baseDic ),
	filterDic_( 0 ),
	levDEA_( 0 ), // Here the default levenshtein threshold is specified
	minNrOfPatterns_( 0 ),
	maxNrOfPatterns_( Vaam::INFINITE )
    {
	patternGraph_.loadPatterns( patternFile );
    }

    
    template< typename MinDicType >
    void Vaam< MinDicType >::setDistance( size_t d ) {
	levDEA_.setDistance( d );
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setMinNrOfPatterns( size_t n ) {
	minNrOfPatterns_ = n;
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setMaxNrOfPatterns( size_t n ) {
	maxNrOfPatterns_ = n;
    }

    template< typename MinDicType >
    bool Vaam< MinDicType >::query( const std::wstring& word, iVaamResultReceiver* interpretations ) const {
	query_ = word;
	interpretations_ = interpretations;
	foundAnswers_ = false;
	
	if( ! baseDic_ ) throw csl::exceptions::LogicalError( "csl::Vaam::query: No baseDic_ loaded." );
	if( ! baseDic_->readyToRead() ) throw csl::exceptions::LogicalError( "csl::Vaam::query: baseDic_ not ready to read." );

	levDEA_.loadPattern( query_.c_str() );
	stack_.clear();
	// create a first StackItem
	stack_.push_back( StackItem( *this ) );
	// insert an initial position at the the levDEA's start state, with 0 applied patterns
	stack_.at( 0 ).push_back( Position( levDEA_.getRoot(), 0 ) );
	// set the patternPos to the graph's root
	stack_.at( 0 ).patternPos_ = patternGraph_.getRoot();

	query_rec( 0 );

	return foundAnswers_;
    }
    
    template< typename MinDicType >
    void Vaam< MinDicType >::setFilterDic( iDictionary const& filterDic ) {
	filterDic_ = &filterDic;
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setBaseDic( MinDic_t const& filterDic ) {
	baseDic_ = &filterDic;
    }


    template< typename MinDicType >
    void Vaam< MinDicType >::query_rec( size_t depth ) const {
	
 	// std::wcout<<"query_rec( "<<depth<<" ):word="<<baseWord_<<std::endl; // DEBUG


	stack_.push_back( StackItem( *this ) );

	// this lets us use the []-operator for stack_ with a somewhat purer conscience
	// But the at()-operator turned out to be a real bottleneck here ...
	if( stack_.size() != depth + 2 ) {
	    throw exceptions::LogicalError( "csl::Vaam::query_rec: current stack seems out of sync with backtracking procedure" );
	}

	// apply all patterns whose left sides end here 
	if( stack_[depth].patternPos_.isFinal() ) {
	    // a copy is needed here, because the state mus descend the errorlinks in order to find more matches (-->Aho-Corasick alg.)
	    PatternGraph::State patPos = stack_[depth].patternPos_;
	    
	    
	    do { // for all final states reachable via errorLinks
		assert( patPos.isFinal() );
		size_t count = 0;

		// for all positions of the stackItem (tracked back leftside)
	    	for( typename StackItem::iterator position = stack_.at( depth - patPos.getDepth() ).begin();
		     position != stack_.at( depth - patPos.getDepth() ).end();
		     ++position, ++count ) {
		    
		    // check if maxNrOfPatterns_ is reached already
		    if( ( maxNrOfPatterns_ != Vaam::INFINITE ) && ( position->getNrOfPatternsApplied() == maxNrOfPatterns_ ) )
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
	    ( stack_[depth].dicPos_.isFinal() ) // arrived at a final state of the dic AND
	    ) {
	    // for all positions
	    size_t count = 0;
	    for( typename StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position ) {
		if( levDEA_.isFinal( position->levPos_ ) ) {
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
	    stack_.at( depth + 1 ).clear();

	    stack_[depth + 1].patternPos_ = stack_[depth].patternPos_.getTransTarget( *c );

	    // see which of the Positions can be moved with this *c
	    // see also if the levDEA reaches a final state
	    size_t count = 0;
	    for( typename StackItem::iterator position = stack_[depth].begin();
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
		baseWord_.resize( depth + 1 );
		baseWord_.at( depth ) = *c;

		query_rec( depth + 1 );
	    }
	}
	
	stack_.pop_back();
    } // query_rec

    template< typename MinDicType >
    void Vaam< MinDicType >::reportMatch( const Position* cur, int baseWordScore ) const {
	Interpretation interpretation;

	interpretation.setBaseWord( baseWord_ ); 
	reportMatch_rec( cur, &interpretation );
	interpretation.setLevDistance( levDEA_.getDistance( cur->levPos_ ) );
	interpretation.setBaseWordScore( baseWordScore );

	// find out what word we're talking about by applying the pattern to the baseWord
	std::wstring word = baseWord_;
	interpretation.getInstruction().applyTo( &word );
	interpretation.setWord( word ); 

	if( ( filterDic_ && filterDic_->lookup( interpretation.getWord() ) )  || // if there's a filterDic_ and interpretation.word is in it or ..
	    ( interpretation.getInstruction().size() < minNrOfPatterns_ )
	    
	    ) {
	    return;
	}

	foundAnswers_ = true;
	interpretations_->receive( interpretation );
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::reportMatch_rec( const Position* cur, Interpretation* interpretation ) const {
	if( cur->mother_.first == -1 ) {
	    return;
	}
	else {
	    reportMatch_rec( &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ), interpretation );
	}

	if( ! cur->posPattern_.empty() ) {
	    interpretation->getInstruction().push_back( cur->posPattern_ );
	}
    }


    
} // namespace csl
#endif
