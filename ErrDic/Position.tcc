namespace csl {

    PatternApplier::Position::Position() :
	state_( 0 ),
	nextChar_( 0 ),
	errorsOrDepth_( 0 ),
	nextItem_( 0 )
    {
    }
    
    PatternApplier::Position::Position( PosType type, StateId_t state, const wchar_t* nextChar, size_t errorsOrDepth ) :
	posType_( type ),
	state_( state ),
	nextChar_( nextChar ),
	errorsOrDepth_( errorsOrDepth ),
	nextItem_( 0 )
    {
    }

    
    void PatternApplier::Position::set( PosType type, StateId_t state, const wchar_t* nextChar, size_t errorsOrDepth ) {
	posType_ = type;
	state_ = state;
	nextChar_ = nextChar;
	errorsOrDepth_ = errorsOrDepth;
	nextItem_ = 0;
    }

    StateId_t PatternApplier::Position::getState() const {
	return state_;
    }

    size_t PatternApplier::Position::hasError() const {
	if( posType_ != DIC ) throw exceptions::LogicalError( "PatternApplier::Position::hasError() called for Position of type other than DIC" );
	return errorsOrDepth_;
    }

    size_t PatternApplier::Position::getDepth() const {
	if( posType_ != PATTRACER ) throw exceptions::LogicalError( "PatternApplier::Position::getDepth() called for Position of type other than PATTRACER" );
	return errorsOrDepth_;
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
