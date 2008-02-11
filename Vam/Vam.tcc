namespace csl {

    Vam::Vam( const MinDic_t& baseDic, const char* patternFile ) :
	baseDic_( baseDic )
    {
	patternGraph_.loadPatterns( patternFile );
    }

    
    void Vam::setDistance( size_t d ) {
	levDEA_.setDistance( d );
    }

    void Vam::query( const std::wstring& word, std::vector< Answer >* answers ) {
	query_ = word;
	answers_ = answers;
	
	levDEA_.loadPattern( query_.c_str() );
	stack_.clear();
	// create a first StackItem
	stack_.push_back( StackItem( *this ) );
	// insert an initial position at the the levDEA's start state
	stack_.at( 0 ).push_back( Position( levDEA_.getRoot() ) );
	// set the patternPos to the graph's root
	stack_.at( 0 ).patternPos_ = patternGraph_.getRoot();

	query_rec( 0 );
    }
    
    void Vam::query_rec( size_t depth ) {

// 	std::wcout<<"query_rec( "<<depth<<" ):word="<<word_<<std::endl;

	// apply all patterns whose left sides end here 
	if( stack_.at( depth ).patternPos_.isFinal() ) {
	    PatternGraph::State patPos = stack_.at( depth ).patternPos_;

	    do { // for all final states reachable via errorLinks
		// for all positions of the stackItem (tracked back leftside)

		assert( patPos.isFinal() );
		size_t count = 0;
	    	for( StackItem::iterator position = stack_.at( depth - patPos.getDepth() ).begin();
		     position != stack_.at( depth - patPos.getDepth() ).end();
		     ++position, ++count ) {
		    
		    // for all right sides fitting the current leftSide
		    // Note that there might be final states with empty rightSides, namely those where a suffix
		    // of the current path leads to a "real" final state with non-empty rightSides
		    for( RightList_t::const_iterator rightSide = patPos.getRightSides().begin();
			 rightSide != patPos.getRightSides().end();
			 ++rightSide ) {
			
			LevDEA::Pos newLevPos = levDEA_.walkStr( position->levPos_, rightSide->c_str() );
			if( newLevPos.isValid() )  {
			    Position newPosition( newLevPos, std::make_pair( depth - patPos.getDepth(), count ) );
			    newPosition.addError( Error( patPos.getWord() + L" " + *rightSide, depth - patPos.getDepth() ) );
			    stack_.at( depth ).push_back( newPosition );
			}
			else {
			}
		    } // for all rightSides
		} // for all positions

		patPos.walkErrorLink();
	    } while( patPos.isValid() && patPos.isFinal() && ( patPos.getDepth() >= stack_.at( depth ).lookAheadDepth_ ) );

	} // if found left pattern side


	// report matches
	if( ( stack_.at( depth ).lookAheadDepth_ == 0 ) && stack_.at( depth ).dicPos_.isFinal() ) {

	    // for all positions
	    size_t count = 0;
	    for( StackItem::iterator position = stack_.at( depth ).begin();
		 position != stack_.at( depth ).end();
		 ++position ) {
		if( levDEA_.isFinal( position->levPos_ ) ) {
		    std::wcout<<"[Result] "<<word_<<"|";
		    const Position* cur = &( *position );
		    
		    while( cur ) {
			if( cur->error_.isValid() ) {
			    std::wcout << "(" << cur->error_.getPattern() << "," << cur->error_.getPosition() << ")";
			}
			
			cur = ( cur->mother_.first == -1 ) ? 0 : &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ); 
		    }
		    std::wcout << "|d=" << levDEA_.getDistance( position->levPos_ ) <<  std::endl;
		}
		++count;
	    } // for all positions
	} // report matches
	

	stack_.push_back( StackItem( *this ) );
	
	// for all outgoing transitions
	for( const wchar_t* c = stack_.at( depth ).dicPos_.getSusoString();
	     *c;
	     ++c ) {
	    stack_.at( depth + 1 ).clear();
	    stack_.at( depth + 1 ).dicPos_ = stack_.at( depth ).dicPos_.getTransTarget( *c );
	    word_.resize( depth + 1 );
	    word_.at( depth ) = *c;

	    stack_.at( depth + 1 ).patternPos_ = stack_.at( depth ).patternPos_.getTransTarget( *c );

	    // see which of the Positions can be moved with this *c
	    // see also if the levDEA reaches a final state
	    size_t count = 0;
	    for( StackItem::iterator position = stack_.at( depth ).begin();
		 position != stack_.at( depth ).end();
		 ++position, ++count ) {
		LevDEA::Pos newLevPos = levDEA_.walk( position->levPos_, *c );
		if( newLevPos.isValid() ) {
		    stack_.at( depth + 1 ).push_back( Position( newLevPos, std::make_pair( depth, count ) ) );
		}
	    }
	    
	    // set lookAheadDepth_ for next stackItem: 
	    if( ! stack_.at( depth + 1 ).empty() ) {
		// set to 0 if new positions exist
		stack_.at( depth + 1 ).lookAheadDepth_ = 0;
	    }
	    else {
		// increment if no new positions exist
		stack_.at( depth + 1 ).lookAheadDepth_ = stack_.at( depth ).lookAheadDepth_ + 1;
	    }

	    if( stack_.at( depth + 1 ).lookAheadDepth_ <= stack_.at( depth + 1 ).patternPos_.getDepth() ) {
		query_rec( depth + 1 );
	    }
	}
	
	stack_.pop_back();
    } // query_rec

    void Vam::printPosition( const Position& pos ) const {
	const Position* cur = &( pos );
	std::wcout << "{" << pos.levPos_.position() << "}";
	while( cur ) {
	    if( cur->error_.isValid() ) {
		std::wcout << "(" << cur->error_.getPattern() << "," << cur->error_.getPosition() << ")";
	    }
	    
	    cur = ( cur->mother_.first == -1 ) ? 0 : &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ); 
	}
	std::wcout << "|d=" << levDEA_.getDistance( pos.levPos_ ) <<  std::endl;
    }
    
} // namespace csl
