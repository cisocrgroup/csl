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
	stack_.push_back( StackItem( *this ) );
	stack_.at( 0 ).push_back( Position( levDEA_.getRoot() ) );
	query_rec( 0 );
    }
    
    void Vam::query_rec( size_t depth ) {
	// see if we can find a left pattern side from here backwards

	// the pair is <id,length> for each left side
	std::vector< std::pair< size_t, size_t > > completeLeftSides;
	if( depth > 0 ) {
	    MDState_t dicPos = baseDic_.getRootState();
	    std::wstring::iterator wordPos = word_.begin() + depth;
	    size_t length = 0;
	    do {
		dicPos.walk( *wordPos );
		++length;
		if( dicPos.isFinal() ) {
		    completeLeftSides.push_back( std::make_pair( dicPos.getAnnotation(), length ) );
		}
	    } while( dicPos.isValid() && wordPos != word_.begin() );
	    
	    // for all found left sides
	    for( std::vector< std::pair< size_t, size_t > >::const_iterator leftSide= completeLeftSides.begin();
		 leftSide != completeLeftSides.end();
		 ++leftSide ) {
		
		// for all positions of the stackItem (tracked back leftside)
	    	for( StackItem::iterator positions = stack_.at( depth - leftSide->second ).begin();
		     positions != stack_.at( depth - leftSide->second ).end();
		     ++positions ) {
		    
		    // for all right sides fitting the current leftSide
		    for( RightList_t::const_iterator rightSide = rightSides_.at( leftSide->first ).begin();
			 rightSide != rightSides_.at( leftSide->first ).end();
			 ++rightSide ) {

			LevDEA::Pos newLevPos = levDEA_.walkStr( positions->levPos_, rightSide->c_str() );
			if( newLevPos.isValid() )  {
			    Position newPosition( newLevPos, &( *positions ) );
			    newPosition.addError( new Error( leftSidesList_.at( leftSide->first ) + L" " + *rightSide, 42 ) );
			    stack_.at( depth ).push_back( newPosition );
			}
			
		    } // for all rightSides
		} // for all positions
	    } // for all found left sides
	} // if depth > 0

	
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
	    for( StackItem::iterator positions = stack_.at( depth ).begin();
		 positions != stack_.at( depth ).end();
		 ++positions ) {
		LevDEA::Pos newLevPos = levDEA_.walk( positions->levPos_, *c );
		if( newLevPos.isValid() ) {
		    stack_.at( depth + 1 ).push_back( Position( newLevPos, &( *positions ) ) );
		    
		    if( stack_.at( depth + 1 ).dicPos_.isFinal() && levDEA_.isFinal( newLevPos ) ) {
			std::wcout<<word_<<std::endl;
		    }


		}
	    }
	    
	    // if some could be moved, dig deeper ...
	    if( ! stack_.at( depth + 1 ).empty() ) {


		query_rec( depth + 1 );
	    }

	}
	
    } // query_rec

} // namespace csl
