#include<iostream>

#include<vector>
#include<stack>


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
		hasError_( false ) {
	    }
		
	    Position( StateId_t state, const wchar_t* nextChar, bool hasError, Position* backPtr ) :
		state_( state ),
		nextChar_( nextChar ),
		hasError_( hasError ),
		backPtr_( backPtr ) {
	    }

	    bool operator<( const Position& other ) const {
		return this->getNextChar() < other.getNextChar();
	    }

	    void set(  StateId_t state, const wchar_t* curChar, bool hasError ) {
		state_ = state;
		nextChar_ = curChar;
		hasError_ = hasError;
	    }

	    const StateId_t& getState() const {
		return state_;
	    }

	    size_t hasError() const {
		return hasError_;
	    }

	    wchar_t getNextChar() const {
		return *nextChar_;
	    }

	    bool increaseNextChar() {
		if( *nextChar_ == 0 ) return false;
		++nextChar_;
	    }
	    
	    const Position* getBackPtr() const {
		return backPtr_;
	    }

	    StateId_t state_;
	    const wchar_t* nextChar_;
	    bool hasError_;
	    Position* backPtr_;
	}; // class Position


    public:
	PatternApplier( const MinDic< int >& dic, const MinDic< int >& filterDic, const std::wstring& patternFrom, const std::wstring& patternTo ) :
	    dic_( dic ),
	    filterDic_( filterDic ),
	    data_( 2 ),
	    curDepth_( 0 ),
	    word_( Global::lengthOfStr, 0 ),
	    patternFrom_( patternFrom ),
	    patternTo_( patternTo ) {

	    data_.at( 0 ).reserve( 3 ); // make sure the vector does not realloc
	    data_.at( 1 ).reserve( 3 ); // make sure the vector does not realloc

	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    newBranch( st, suso, 0 );
	    filterStack_.push( filterDic_.getRoot() );
	}

	const std::wstring& getWord() const {
	    return word_;
	}

	bool next() {
	    bool foundFinal = false;
	    bool stackNotEmpty = true;
	    wchar_t label = 0;
	    do { // this loop terminates at a final state
		// std::cerr<<"Enter final loop"<<std::endl;
		do { // this loop terminates if one continuation was found or if the stackItem is done
		    // std::cerr<<"Enter conti loop"<<std::endl;

		    bool foundContinuation = false;
		    bool patternApplicable = false;
		    std::sort( data_.at( getCurDepth() ).begin(), data_.at( getCurDepth() ).end() );
		    std::vector< Position >::iterator it = data_.at( getCurDepth() ).begin();
		    label = it->getNextChar();

		    // go through all Positions with that label
		    while( ( it != data_.at( getCurDepth() ).end() ) && 
			   ( it->getNextChar() == label ) ) {
			StateId_t nextState = dic_.walk( it->getState(), label );
			if( nextState ) {
			    addContinuation( nextState, dic_.getSusoString( nextState ), it->hasError(), &( *it ) );
			    if( ! foundContinuation ) { // if continuation wasn't registered before
				foundContinuation = true;
				word_.resize( getCurDepth() + 1 );
				word_.at( getCurDepth() ) = label;
				
				if( ! patternApplicable ) {
				    patternApplicable = isSuffix();
				}

				StateId_t nextFilterState = filterDic_.walk( filterStack_.top(), label );
				filterStack_.push( nextFilterState );

				std::wcout<<word_<<std::endl;
			    } // register continuation for first time
			    
			    if( patternApplicable && ! it->hasError() ) {
				// go back patternTo_.size()-1 steps to Position where patternTo begins
				size_t i;
				const Position* pos;
				for( pos = &( *it ), i = 0;
				     i < patternTo_.size() - 1;
				     ++i ) {
				    pos = pos->getBackPtr();
				    std::wcerr<<"went back to "<<pos->getState()<<std::endl;
				}
				std::wcerr<<"New state is "<<pos->getState()<<std::endl;
				if( StateId_t newState = dic_.walkStr( pos->getState(), 
								       patternFrom_.c_str() ) ) {
				    addContinuation( newState, dic_.getSusoString( newState ), true, 0 );
				}
			    }

			    it->increaseNextChar();
			    
			    
			    foundFinal = 
				it->hasError() &&
				dic_.isFinal( nextState ) && 
				! filterDic_.isFinal( nextFilterState );
			}
			++it;
		    }
		} while( data_.at( getCurDepth() + 1 ).size() == 0 && label != 0 );
		if( label == 0 ) {
		    data_.at( getCurDepth() ).clear();
		    stackNotEmpty = back();
		}
		else {
		    forward();
		}
		
	    } while( !foundFinal && stackNotEmpty );
	    return ( stackNotEmpty );
	}

    private:
	void addContinuation( StateId_t state, const wchar_t* susoStr, bool hasError, Position* backPtr ) {
	    assert( data_.at( getCurDepth() + 1 ).capacity() >= 3 );
	    data_.at( getCurDepth() + 1 ).push_back( Position( state, susoStr, hasError, backPtr ) );
	}
	
	void newBranch( StateId_t state, const wchar_t* susoStr, size_t errors ) {
	    assert( data_.at( getCurDepth() ).capacity() >= 3 );
	    data_.at( getCurDepth() ).push_back( Position( state, susoStr, errors, 0 ) );
	}
	
	void forward() {
	    ++curDepth_;
	    if( data_.size() <= getCurDepth() + 1 ) data_.resize( getCurDepth() + 2 );
	    data_.at( getCurDepth() + 1 ).reserve( 3 );  // make sure the vector does not realloc
//	    std::cerr<<"forward: depth is "<<getCurDepth()<<std::endl;
	}
	    
	bool back() {
	    if( getCurDepth() == 0 ) return false;
	    data_.at( getCurDepth() ).clear();
	    data_.at( getCurDepth() ).reserve( 3 );
	    --curDepth_;
	    word_.resize( getCurDepth() );
//	    std::cerr<<"back: depth is "<<getCurDepth()<<std::endl;
	    return true;
	}
	    
	bool isSuffix() const {
	    if( word_.substr( word_.size() - patternTo_.size(), patternTo_.size() ) == patternTo_ ) {
		std::wcerr<<"isSuffix: "<<word_<<std::endl;
		return true;
	    }
	    else return false;
	}

	std::vector< std::vector< Position > > data_;
	std::vector< std::set< wchar_t > > transitions_;
	std::wstring word_;
	size_t curDepth_;
	size_t getCurDepth() const {
	    return curDepth_;
	}
	
	std::stack< StateId_t > filterStack_;


	const MinDic< int >& dic_;
	const MinDic< int >& filterDic_;

	// the error pattern
	std::wstring patternFrom_;
	std::wstring patternTo_;

    }; // class PatternApplier




} // eon

int main( int argc, char** argv ) {
    csl::MinDic< int > dic( argv[1]);
    csl::MinDic< int > filterDic( argv[2]);

    csl::PatternApplier pa( dic, filterDic, std::wstring( L"lb" ), std::wstring( L"x" ) );
    
    
    while( pa.next() ) {
	std::wcout<<"OUT: "<<pa.getWord()<<std::endl;
	// std::cerr<<"Enter next-loop"<<std::endl;

    }    
}

