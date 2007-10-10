namespace csl {

    PatternApplier::Position::Position() :
	state_( 0 ),
	nextChar_( 0 ),
	hasError_( false ),
	perfHashValue_( 0 )
    {
    }
    
    PatternApplier::Position::Position( StateId_t state, const wchar_t* nextChar, bool hasError, size_t perfHashValue ) :
	state_( state ),
	nextChar_( nextChar ),
	hasError_( hasError ),
	perfHashValue_( perfHashValue )
    {
    }
    
    void PatternApplier::Position::set(  StateId_t state, const wchar_t* nextChar, bool hasError, size_t perfHashValue ) {
	state_ = state;
	nextChar_ = nextChar;
	hasError_ = hasError;
	perfHashValue_ = perfHashValue;
    }

    StateId_t PatternApplier::Position::getState() const {
	return state_;
    }

    size_t PatternApplier::Position::hasError() const {
	return hasError_;
    }

    wchar_t PatternApplier::Position::getNextChar() const {
	return *nextChar_;
    }

    bool PatternApplier::Position::stepToNextChar() {
	if( *nextChar_ == 0 ) return false;
	++nextChar_;
	return true;
    }

    size_t PatternApplier::Position::getPHValue() const {
	return perfHashValue_;
    }

	    

} // eon
