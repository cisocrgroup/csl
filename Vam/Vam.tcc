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
	// see if we can find a left pattern side from here backwards

	if( stack_.at( depth ).patternPos_.isFinal() ) {
	    PatternGraph::State patPos = stack_.at( depth ).patternPos_;

	    do { // for all final states reachable via errorLinks
		// for all positions of the stackItem (tracked back leftside)
		size_t count = 0;
	    	for( StackItem::iterator positions = stack_.at( depth - patPos.getDepth() ).begin();
		     positions != stack_.at( depth - patPos.getDepth() ).end();
		     ++positions, ++count ) {
		    
		    // for all right sides fitting the current leftSide
		    for( RightList_t::const_iterator rightSide = patPos.getRightSides().begin();
			 rightSide != patPos.getRightSides().end();
			 ++rightSide ) {
			
			LevDEA::Pos newLevPos = levDEA_.walkStr( positions->levPos_, rightSide->c_str() );
			if( newLevPos.isValid() )  {
			    Position newPosition( newLevPos, std::make_pair( depth - patPos.getDepth(), count ) );
			    newPosition.addError( Error( patPos.getWord() + L" " + *rightSide, depth - patPos.getDepth() ) );
			    stack_.at( depth ).push_back( newPosition );
			}
		    } // for all rightSides
		} // for all positions

		do {
		    patPos.walkErrorLink();
		} while( patPos.isValid() && ! patPos.isFinal() );
		
	    } while( patPos.isValid() );
	} // if found left pattern side


	// report matches
	if( stack_.at( depth ).dicPos_.isFinal() ) {
	    for( StackItem::iterator position = stack_.at( depth ).begin();
		 position != stack_.at( depth ).end();
		 ++position ) {
		if( levDEA_.isFinal( position->levPos_ ) ) {
		    std::wcout<<word_<<"|";
		    const Position* cur = &( *position );

		    while( cur ) {
			if( cur->error_.isValid() ) {
			    std::wcout << "(" << cur->error_.getPattern() << "," << cur->error_.getPosition() << ")";
			}
			
			cur = ( cur->mother_.first == -1 ) ? 0 : &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ); 
		    }
		    std::wcout << "|d=" << levDEA_.getDistance( position->levPos_ ) <<  std::endl;
		}
	    }
	}


	// ** RECURSIVE PART **
	// for all outgoing transitions
	stack_.push_back( StackItem( *this ) );
	for( const wchar_t* c = stack_.at( depth ).dicPos_.getSusoString();
	    *c;
	     ++c ) {

	    stack_.at( depth + 1 ).clear();
	    stack_.at( depth + 1 ).dicPos_ = stack_.at( depth ).dicPos_.getTransTarget( *c );
	    word_.resize( depth + 1 );
	    word_.at( depth ) = *c;

	    // see which of the Positions can be moved with this *c
	    // see also if the levDEA reaches a final state
	    size_t count = 0;
	    for( StackItem::iterator position = stack_.at( depth ).begin();
		 position != stack_.at( depth ).end();
		 ++position, ++count ) {
		LevDEA::Pos newLevPos = levDEA_.walk( position->levPos_, *c );
		if( newLevPos.isValid() ) {
		    stack_.at( depth + 1 ).patternPos_ = stack_.at( depth ).patternPos_.getTransTarget( *c );
		    stack_.at( depth + 1 ).push_back( Position( newLevPos, std::make_pair( depth, count ) ) );
		    
		}
	    }
	    
	    // if some could be moved, dig deeper ...
	    if( ! stack_.at( depth + 1 ).empty() ) {


		query_rec( depth + 1 );
	    }

	}
	
    } // query_rec

} // namespace csl
