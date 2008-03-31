#ifndef CSL_TEST_PATTERN_H
#define CSL_TEST_PATTERN_H CSL_TEST_PATTERN_H

#include "../Pattern.h"
#include "../PatternSet.h"
#include "../PosPattern.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    /**
     * This test class subsumes tests for the classes Pattern, PosPattern and Instruction
     * @author Ulrich Reffle, 2008
     */
    class TestPattern : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestPattern );
	CPPUNIT_TEST( testPattern );
	CPPUNIT_TEST( testPatternSet );
	CPPUNIT_TEST( testPosPattern );
	CPPUNIT_TEST_SUITE_END();
    public:

	void testPattern();
	void testPatternSet();
	void testPosPattern();


    private:
	
    }; // class TestPattern

    CPPUNIT_TEST_SUITE_REGISTRATION( TestPattern );

    /**
     * test the basic methods of class Pattern.
     */
    void TestPattern::testPattern() {
	// test constructor and the getters. 
	csl::Pattern p1( L"left", L"right" );
	CPPUNIT_ASSERT( p1.getLeft() == L"left" );
	CPPUNIT_ASSERT( p1.getRight() == L"right" );
    }

    void TestPattern::testPatternSet() {
	csl::PatternSet pSet;
	pSet.loadPatterns( "./test.patterns.txt" );
    }


    void TestPattern::testPosPattern() {
	csl::Pattern p1( L"left", L"right" );

	// construct PosPattern from existing Pattern
	csl::PosPattern pp1( p1, 3 );
	CPPUNIT_ASSERT( pp1.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp1.getRight() == L"right" );
	CPPUNIT_ASSERT( pp1.getPosition() == 3 );

	// construct PosPattern from existing Pattern
	csl::PosPattern pp2( L"left", L"right", 3 );
	CPPUNIT_ASSERT( pp2.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp2.getRight() == L"right" );
	CPPUNIT_ASSERT( pp2.getPosition() == 3 );
	

    }


} // namespace csl


#endif
