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

	class Stack {
	public:
	    Stack() :
		data_( 1 ),
		curDepth_( 0 ) {
	    }

	    void addContinuation( StateId_t state, const wchar_t* susoStr, size_t errors ) {
		data_.at( getCurDepth() + 1 ).push_back( Position( state, susoStr, errors ) );
	    }

	    void newBranch( StateId_t state, const wchar_t* susoStr, size_t errors ) {
		data_.at( getCurDepth() ).push_back( Position( state, susoStr, errors ) );
	    }

	    std::vector< Position>::iterator getMinPosition

	    void forward() {
		if( data_.size() <= getCurDepth() + 1 ) data_.resize( data_.size() + 1 );
		++curDepth_;
	    }
	    
	    void back() {
		data_.at( getCurDepth() ).clear();
		--curDepth_;
	    }
	    
	    
	private:
	    std::vector< std::vector< Position > > data_;
	    std::vector< wchar_t > labels_;
	    size_t curDepth_;
	    size_t getCurDepth() const {
		return curDepth_;
	    }
	};

    public:
	PatternApplier( char* minDicFile ) :
	    dic_( minDicFile ) {

	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    stack_.newBranch( st, suso, 0 );
	    
	}

	void getNext() {
	    std::vector< Position>::iterator it = std::min_element( data_.at( getCurDepth() ).begin(), data_.at( getCurDepth() ).end() );
	}

    public:
	MinDic< int > dic_;


    private:
	Stack stack_;
    };




} // eon

int main( int argc, char** argv ) {
    csl::PatternApplier pa( argv[1] );
    
    
    pa.getNext();
    
}

