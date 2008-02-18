namespace csl {

    PatternApplier::Position::Position() :
	state_( 0 ),
	nextChar_( 0 )
    {
    }
    
    PatternApplier::Position::Position( const MDState_t& state ) :
	state_( state ),
	nextChar_( state_.getSusoString() )
    {
    }
    
//     void PatternApplier::Position::set(  const MDState_t& state, const wchar_t* nextChar, const Error& error, size_t perfHashValue ) {
// 	state_ = state;
// 	nextChar_ = nextChar;
// 	error_ = error;
// 	perfHashValue_ = perfHashValue;
//     }

    const PatternApplier::MDState_t& PatternApplier::Position::getState() const {
	return state_;
    }

    size_t PatternApplier::Position::hasErrors() const {
	return ! errors_.empty();
    }

    wchar_t PatternApplier::Position::getNextChar() const {
	return *nextChar_;
    }

    bool PatternApplier::Position::stepToNextChar() {
	if( *nextChar_ == 0 ) return false;
	++nextChar_;
	return true;
    }

} // eon
