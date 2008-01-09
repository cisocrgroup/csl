namespace csl {

    PatternApplier::Error::Error() :
	patternID_( -1 ),
	position_( -1 ) {
    }


    PatternApplier::Error::Error( size_t errorPattern, size_t position ) :
	patternID_( errorPattern ),
	position_( position ) {
    }

    void PatternApplier::Error::set( size_t errorPattern, size_t position ) {
	patternID_ = errorPattern;
	position_ = position;
    }
	    
    size_t PatternApplier::Error::getPosition() const {
	return position_;
    }

    size_t PatternApplier::Error::getPatternID() const {
	return patternID_;
    }

}
