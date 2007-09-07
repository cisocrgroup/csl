#include<iostream>

#include "../Global.h"
#include "../MinDic/MinDic.h"

namespace csl {

    class PatternApplier {
    private:
	class Position {
	    public:
	    Position() :
		state_( 0 ),
		nextChar_( 0 ),
		errors_( 0 ) {
	    }
		
	    Position( StateId_t state, const wchar_t* nextChar, size_t errors ) :
		state_( state ),
		nextChar_( nextChar ),
		errors_( errors ) {
	    }

	    bool operator<( const Position& other ) const {
		return this->getNextChar() < other.getNextChar();
	    }

	    void set(  StateId_t state, const wchar_t* curChar, size_t errors ) {
		state_ = state;
		nextChar_ = curChar;
		errors_ = errors;
	    }

	    const StateId_t& getState() const {
		return state_;
	    }

	    size_t getErrors() const {
		return errors_;
	    }

	    wchar_t getNextChar() const {
		return *nextChar_;
	    }

	    bool increaseNextChar() {
		if( *nextChar_ == 0 ) return false;
		++nextChar_;
	    }
	    
	    StateId_t state_;
	    const wchar_t* nextChar_;
	    size_t errors_;
	};


	void addContinuation( StateId_t state, const wchar_t* susoStr, size_t errors ) {
	    data_.at( getCurDepth() + 1 ).push_back( Position( state, susoStr, errors ) );
	}
	
	void newBranch( StateId_t state, const wchar_t* susoStr, size_t errors ) {
	    data_.at( getCurDepth() ).push_back( Position( state, susoStr, errors ) );
	}
	

	void forward() {
	    if( data_.size() <= getCurDepth() + 1 ) data_.resize( data_.size() + 1 );
	    ++curDepth_;
	}
	    
	void back() {
	    data_.at( getCurDepth() ).clear();
	    --curDepth_;
	}
	    
	std::vector< std::vector< Position > > data_;
	std::wstring word_;
	size_t curDepth_;
	size_t getCurDepth() const {
	    return curDepth_;
	}

	MinDic< int > dic_;

    public:
	PatternApplier( char* minDicFile ) :
	    dic_( minDicFile ),
	    data_( 1 ) {

	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    newBranch( st, suso, 0 );
	}

	void getNext() {
	    bool foundFinal = false;
	    wchar_t label = 0;
	    do {
		std::sort( data_.at( getCurDepth() ).begin(), data_.at( getCurDepth() ).end() );
		std::vector< Position >::iterator it = data_.at( getCurDepth() ).begin();
		label = it->getNextChar();
		while( it->getNextChar() == label ) {
		    StateId_t nextState = dic_.walk( it->getState(), label );
		    if( nextState ) {
			addContinuation( nextState, dic_.getSusoString( nextState ), it->getErrors() );
			word_.at( getCurDepth() ) = label;
			it->increaseNextChar();
		    }
		}
	    } while( data_.at( getCurDepth() + 1 ).size() == 0 && label != 0 );

	    if( label == 0 ) {
		data_.at( getCurDepth() ).clear();
		
	    }
	}
 



    }; // class PatternApplier




} // eon

int main( int argc, char** argv ) {
    csl::PatternApplier pa( argv[1] );
    
    
    pa.getNext();
    
}

