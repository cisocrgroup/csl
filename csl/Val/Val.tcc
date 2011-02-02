namespace csl {

    Val::Val( MinDic_t const& baseDic, char const* patternFile ) :
	patternGraph_( PatternGraph::FORWARD, PatternGraph::INDEX_RIGHT ),
	caseMode_( Global::asIs ),
	locale_( CSL_UTF8_LOCALE ) {

	baseDic_ = &baseDic;
	patternGraph_.loadPatterns( patternFile );


    }


    void Val::setMinNrOfPatterns( size_t n ) {
	minNrOfPatterns_ = n;
    }

    void Val::setMaxNrOfPatterns( size_t n ) {
	maxNrOfPatterns_ = n;
    }


    inline bool Val::query( std::wstring const& word, iInterpretationReceiver* interpretations ) const {
	query_ = word;

	wasUpperCase_ = false;
	if( ( caseMode_ != Global::asIs ) && std::iswupper( query_.at( 0 )/*, locale_*/ ) ) {
	    wasUpperCase_ = true;
	    query_.at( 0 ) = std::tolower( query_.at( 0 ), locale_ );
	}

	interpretations_ = interpretations;
	
	stack_.clear();
	stack_.reserve( query_.length() + 1 );
	stack_.push_back( StackItem() );
	stack_.back().push_back( Position( baseDic_->getRootState() ) );
	

	size_t depth = 1; // depth 0 was initialized above
	PatternGraph::State patternPos = patternGraph_.getRoot();

	// iterate through query
	for( std::wstring::const_iterator c = query_.begin(); c != query_.end(); ++c, ++depth ) {
	    
	    StackItem const& last = stack_.back();
	    stack_.push_back( StackItem() );
	    StackItem& cur = stack_.back();

	    // std::wcout << "depth=" << depth << ",#pos=" << last.size() << ", patPos=" << patternPos.getStateIndex() << std::endl;

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
		
	    } // if patternPos final

	    if( cur.empty() ) {
		cur.lookAheadDepth_ = last.lookAheadDepth_ + 1;
		if( cur.lookAheadDepth_ > patternPos.getDepth() ) {
		    //std::wcerr << "break" << std::endl;
		    break;
		}
	    }
	    //std::wcerr << "lad=" << cur.lookAheadDepth_ << std::endl;

	    //std::wcout << "sizeof_bucket=" << cur.size() << std::endl;
	    
	} // for all chars of the query word

	// report matches
	bool foundAnswers = false;
	if( ( depth == word.length() + 1 ) && ( stack_.back().lookAheadDepth_ == 0 ) ) {  // we are not in the lookahead-phase
	    // for all positions
	    size_t count = 0;
	    for( StackItem::iterator position = stack_.back().begin();
		 position != stack_.back().end();
		 ++position ) {
		if( position->dicPos_.isFinal() ) {
		    reportMatch( &( *position ), position->dicPos_.getAnnotation() );
		    foundAnswers = true;
		}
		++count;
	    } // for all positions
	} // report matches
	return foundAnswers;
    } // method query
    
    void Val::reportMatch( const Position* cur, int baseWordScore ) const {
	Interpretation interpretation;

	reportMatch_rec( cur, &interpretation );
	interpretation.setLevDistance( 0 );
	interpretation.setBaseWordScore( baseWordScore );

	interpretation.setWord( query_ ); 
	// find out what baseWord we're talking about by applying the pattern to the query
	std::wstring word = query_;
	interpretation.getInstruction().applyTo( &word, -1 );
	interpretation.setBaseWord( word ); 

	// *** if you activate this, take care of the return value of query (stating if matches were found)***
// 	if( ( filterDic_ && filterDic_->lookup( interpretation.getWord() ) )  || // if there's a filterDic_ and interpretation.word is in it or ..
// 	    ( interpretation.getInstruction().size() < minNrOfPatterns_ )
	    
// 	    ) {
// 	    return;
// 	}

	if( wasUpperCase_ ) {
	    
	    std::wstring tmp = interpretation.getBaseWord();
	    tmp.at( 0 ) = std::toupper( tmp.at( 0 ), locale_ );
	    interpretation.setBaseWord( tmp );

	    tmp = interpretation.getWord();
	    tmp.at( 0 ) = std::toupper( tmp.at( 0 ), locale_ );
	    interpretation.setWord( tmp );
	}


 	interpretations_->receive( interpretation );
   }

    int Val::reportMatch_rec( const Position* cur, Interpretation* interpretation ) const {
	int lengthDiff = 0;
	if( cur->mother_.first == -1 ) {
	    return 0;
	}
	else {
	    //std::wcout << cur->mother_.first << "," << cur->mother_.second << "," << stack_.at( cur->mother_.first ).size() << std::endl;
	    lengthDiff = reportMatch_rec( &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ), interpretation );
	}

	if( ! cur->posPattern_.empty() ) {
	    interpretation->getInstruction().push_back( cur->posPattern_ );
	    interpretation->getInstruction().back().setPosition( cur->posPattern_.getPosition() - lengthDiff );
	    lengthDiff += cur->posPattern_.getRight().size() - cur->posPattern_.getLeft().size(); 
	}
	return lengthDiff;
    }

    
} // eon
