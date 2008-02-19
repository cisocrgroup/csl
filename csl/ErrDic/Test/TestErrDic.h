#ifndef CSL_TESTERRDIC_H
#define CSL_TESTERRDIC_H CSL_TESTERRDIC_H

#include "../../Global.h"
#include "../ErrDic.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestErrDic : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestErrDic );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestErrDic();

	void run();

	void testBasics();
// 	void testConstructionDetails();
// 	void lookupAllKeys();


    private:
	csl::ErrDic<> errdic_;
	
    };

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestErrDic::testBasics() {
	std::wstring key( L"aachen" );
	/*
	 * walk key char by char and test if
	 * - key could be walked completely
	 * - pos_1 is a valid state of the automaton
	 * - pos_1 is a final state
	 */
	StateId_t pos_1 = mdic_.getRoot(); // set pos_1 to root
	const wchar_t* c = 0;
	for( c = key.c_str(); *c && pos_1; ++c ) {
	    pos_1 = mdic_.walk( pos_1, *c );
	}

	CPPUNIT_ASSERT( ! *c );
	CPPUNIT_ASSERT( pos_1 );
	assert( mdic_.isFinal( pos_1 ) );
	int ann = 0;

	/*
	 * test if the same result is returned by walkStr
	 */
	uint_t pos_2 = mdic_.walkStr( mdic_.getRoot(), key.c_str() );
	CPPUNIT_ASSERT( pos_1 == pos_2 );
	
	/*
	 * test if walking with the empty string works
	 */
	key = L"";
	CPPUNIT_ASSERT( mdic_.walkStr( mdic_.getRoot(), key.c_str() ) == mdic_.getRoot() );

	CPPUNIT_ASSERT( 0 );

} // namespace csl


#endif
