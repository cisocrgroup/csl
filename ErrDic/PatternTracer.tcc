namespace csl {

    inline PatternApplier::PatternTracer::PatternTracer() :
	state_( 0 ),
	nextChar_( 0 ),
	depth_( 0 ) {
    }

    inline PatternApplier::PatternTracer::PatternTracer( const MDState_t& state, size_t depth ) :
	state_( state ),
	depth_( depth ) {
	nextChar_ = state.getSusoString();

	// is that for deletions???
	if( *nextChar_ == PatternApplier::patternDelimiter_ ) {
	    ++nextChar_;
	}
    }

    inline PatternApplier::PatternTracer::~PatternTracer() {}

    inline bool PatternApplier::PatternTracer::isPosition() const {
	return false;
    }

    inline bool PatternApplier::PatternTracer::isPatternTracer() const {
	return true;
    }

    inline wchar_t PatternApplier::PatternTracer::getNextChar() const {
	return *nextChar_;
    }

    bool PatternApplier::PatternTracer::stepToNextChar() {
	if( *nextChar_ == 0 ) return false;
	++nextChar_;
	if( *nextChar_ == PatternApplier::patternDelimiter_ ) {
	    std::wcout<<"HA"<<std::endl;
	    ++nextChar_;
	    if( *nextChar_ == 0 ) return false;
	}
	return true;
    }

    inline const PatternApplier::MDState_t& PatternApplier::PatternTracer::getState() const {
	return state_;
    }
	    
    inline size_t PatternApplier::PatternTracer::getDepth() const {
	return depth_;
    }
    
}
