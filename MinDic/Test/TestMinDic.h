#ifndef TESTMINDIC_H
#define TESTMINDIC_H TESTMINDIC_H

#include "../../Global.h"
#include "../../Test/Test.h"
#include "../MinDic.h"



class TestMinDic : public csl::Test  {
public:
    TestMinDic() : Test( "csl::MinDic" ),
		   alph_( "./MinDic/Test/test.azb" ),
		   mdic_( alph_ )
	{
	    printf("test\n");
	    try {
	    mdic_.compileDic( "./MinDic/Test/test.lex" );
	    } catch( csl::exceptions::cslException exc ) {
		std::cout<<exc.what()<<std::endl;
		return;
	    }
    }

    bool run() {
	printf("test\n");
	return true;
    }

private:
    csl::Alphabet alph_;
    csl::MinDic mdic_;
};


#endif
