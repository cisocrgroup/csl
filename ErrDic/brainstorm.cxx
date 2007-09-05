#include<iostream>

#include "../Global.h"
#include "../MinDic/MinDic.h"

namespace csl {

    class PatternApplier {
    private:
	class Position {
	    public:
	    Position( StateId_t pos, const wchar_t* nextChar, size_t errors ) :
		pos_( pos ),
		nextChar_( nextChar ),
		errors_( errors ) {
	    }

	    StateId_t pos_;
	    const wchar_t* curChar_;
	    size_t errors_;
	};

	class Stack {
	public:
	    Stack() {
	    }
	    void addPosition( Position pos );
	    

	private:
	    size_t depth_;
	    

	};

    public:
	PatternApplier( char* minDicFile ) :
	    dic_( minDicFile ) {

	    StateId_t st = dic_.getRoot();
	    const wchar_t* suso = dic_.getSusoString( st );

	    stack_.push_back( std::vector< StackItem >( StackItem( st, suso ) ) );
	}

	void getNext() {
	    
	}

    public:
	MinDic< int > dic_;


    private:
	std::vector< std::vector< StackItem > > stack_;

    };




} // eon

int main( int argc, char** argv ) {
    csl::PatternApplier pa( argv[1] );
    
    
    pa.getNext();
    
}

