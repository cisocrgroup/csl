#ifndef CSL_TEST_PATTERN_H
#define CSL_TEST_PATTERN_H CSL_TEST_PATTERN_H

#include "../Pattern.h"
#include "../PatternSet.h"
#include "../PosPattern.h"
#include "../PatternWeights.h"
#include "../ComputeInstruction.h"

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
	CPPUNIT_TEST( testPatternWeights );
	CPPUNIT_TEST( testComputeInstruction );
	CPPUNIT_TEST_SUITE_END();
    public:

	void testPattern();
	void testPatternSet();
	void testPosPattern();
	void testPatternWeights();
	void testComputeInstruction();

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
	//pSet.loadPatterns( "./test.patterns.txt" );
    }

    void TestPattern::testPosPattern() {
	
	// construct PosPattern from existing Pattern
	csl::PosPattern pp2( L"left", L"right", 3 );
	CPPUNIT_ASSERT( pp2.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp2.getRight() == L"right" );
	CPPUNIT_ASSERT( pp2.getPosition() == 3 );
    }

    void TestPattern::testPatternWeights() {
	PatternWeights pw;
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == PatternWeights::UNDEF );
	pw.setWeight( Pattern( L"t", L"th" ), 0.35 );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) );
	
	pw.setDefault( PatternWeights::PatternType( 1, 2 ), 1.3 );
	
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) ); // as before
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"x", L"yz" ) ) == static_cast< float >( 1.3 ) ); // default value
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"xy", L"z" ) ) == PatternWeights::UNDEF ); // as before
    }


    void TestPattern::testComputeInstruction() {
	ComputeInstruction ci;
	ci.setDebug( 1 );
	PatternWeights pw;
	ci.connectPatternWeights( &pw );
	
	pw.setDefault( PatternWeights::PatternType( 0, 1 ), 1 ); // standard ins
	pw.setDefault( PatternWeights::PatternType( 1, 0 ), 1 ); // standard del
	pw.setDefault( PatternWeights::PatternType( 1, 1 ), 1 ); // standard sub

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh" ) == 1 ); // 1 ins

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh" ) == 1 ); // 1 ins at beginning of word

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxh" ) == 1 ); // 1 sub

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xuh" ) == 1 ); // 1 sub at beginning of word


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"nruh" ) == 2 ); // 1 ins, 1 sub

	CPPUNIT_ASSERT( ci.computeInstruction( L"murnau", L"mumau" ) == 2 ); // 1 del, 1 sub

	pw.setWeight( Pattern( L"m", L"rn" ), 0.35 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"rnuh" ) == static_cast< float >( 0.35 ) ); // 1 split

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"rnuhx" ) == static_cast< float >( 1.35 ) ); // 1 split, 1 insertion

	pw.setWeight( Pattern( L"m", L"n" ), 0.54 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mumm", L"nurnm" ) == static_cast< float >( 0.89 ) ); // 1 sub 0.54, 1 split 0.35

	//// THIS WORKS WITH 0.56, BUT NOT WITH 0.57! AAAAAAAAAAAAHHHHHHHHHH
	pw.setWeight( Pattern( L"rn", L"m" ), 0.56 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"murnau", L"mumau" ) == static_cast< float >( 0.56 ) ); // 1 merge 0.56
	//std::wcout<<"HALLO:"<< ci.computeInstruction( L"murnau", L"mmau" ) << std::endl;
	//std::wcout<<"DIFF:"<< ci.computeInstruction( L"murnau", L"mmau" ) - static_cast< float >( 1.56 ) << std::endl;
	CPPUNIT_ASSERT( ci.computeInstruction( L"murnau", L"mmau" ) == static_cast< float >( 1.56 ) ); // 1 del, 1 merge 0.56



    }

} // namespace csl


#endif
