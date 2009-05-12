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

	pw.reset();
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == PatternWeights::UNDEF );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"x", L"yz" ) ) == PatternWeights::UNDEF );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"xy", L"z" ) ) == PatternWeights::UNDEF );


    }


    void TestPattern::testComputeInstruction() {
	ComputeInstruction ci;
	ci.setDebug( 1 );
	PatternWeights pw;
	ci.connectPatternWeights( pw );

	std::vector< Instruction > instructions;
	
	pw.setDefault( PatternWeights::PatternType( 0, 1 ), 1 ); // standard ins
	pw.setDefault( PatternWeights::PatternType( 1, 0 ), 1 ); // standard del
	pw.setDefault( PatternWeights::PatternType( 1, 1 ), 1 ); // standard sub


// 	//////////////////////////////////////////////////////////////////////////////////////////////////7
// 	pw.setDefault( PatternWeights::PatternType( 2, 1 ), 1 ); // standard merge
// 	pw.setDefault( PatternWeights::PatternType( 1, 2 ), 1 ); // standard split
// 	CPPUNIT_ASSERT( ci.computeInstruction( L"abcde", L"abxcde", &instructions ) == 1 );
// 	for( std::vector< Instruction >::const_iterator it = instructions.begin(); it != instructions.end(); ++it ) {
// 	    it->print();std::wcout<<std::endl;
// 	}

// 	instructions.clear();
// 	CPPUNIT_ASSERT( ci.computeInstruction( L"abcde", L"axcdy", &instructions ) == 2 ); // 1 sub

// 	for( std::vector< Instruction >::const_iterator it = instructions.begin(); it != instructions.end(); ++it ) {
// 	    it->print();std::wcout<<std::endl;
// 	}

// 	instructions.clear();
// 	CPPUNIT_ASSERT( ci.computeInstruction( L"abcdey", L"abxcdez", &instructions ) == 2 ); // 1 sub
	
	
// 	return;
// 	//////////////////////////////////////////////////////////////////////////////////////////////////7

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) == 1 ); // 1 ins
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );
	instructions.clear();


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) == 1 ); // 1 ins at beginning of word
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	instructions.clear();


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxh", &instructions ) == 1 ); // 1 sub
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"u", L"x", 1 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xuh", &instructions ) == 1 ); // 1 sub at beginning of word
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"x", 0 ) );
	instructions.clear();


	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"nrilk", &instructions ) == 2 ); // 1 ins, 1 sub
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"m", L"r", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 1 ) == PosPattern( L"", L"r", 1 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"murnau", L"mumau", &instructions ) == 2 ); // 1 del, 1 sub
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"r", L"", 2 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"n", L"m", 3 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"r", L"m", 2 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 1 ) == PosPattern( L"n", L"", 3 ) );
	instructions.clear();

	pw.setWeight( Pattern( L"m", L"rn" ), 0.35 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"rnilk", &instructions ) == static_cast< float >( 0.35 ) ); // 1 split
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"rn", 0 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"rnilkx", &instructions ) == static_cast< float >( 1.35 ) ); // 1 split, 1 insertion
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"rn", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"", L"x", 4 ) );
	instructions.clear();

	pw.setWeight( Pattern( L"m", L"n" ), 0.54 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"milkman", L"nilkrnan", &instructions ) == static_cast< float >( 0.89 ) ); // 1 sub 0.54, 1 split 0.35
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"m", L"rn", 4 ) );
	instructions.clear();

	//// THIS WORKS WITH 0.56, BUT NOT WITH 0.57! AAAAAAAAAAAAHHHHHHHHHH
	pw.setWeight( Pattern( L"rn", L"m" ), 0.56 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"morning", L"moming", &instructions ) == static_cast< float >( 0.56 ) ); // 1 merge 0.56
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"rn", L"m", 2 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"morning", L"mming", &instructions ) == static_cast< float >( 1.56 ) ); // 1 del, 1 merge 0.56
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"o", L"", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"rn", L"m", 2 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"morning", L"moming", &instructions ) == static_cast< float >( 0.56 ) ); // 1 merge 0.56
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"rn", L"m", 2 ) );
	instructions.clear();

	// this is a setup that is used for the Profiler, for instance
	/// obviously the merges and splits give additional trouble
	pw.reset();
	pw.setDefault( csl::PatternWeights::PatternType( 1, 1 ), 1 );
	pw.setDefault( csl::PatternWeights::PatternType( 1, 0 ), 1 );
	pw.setDefault( csl::PatternWeights::PatternType( 0, 1 ), 1 );
	pw.setDefault( csl::PatternWeights::PatternType( 2, 1 ), 1 );
	pw.setDefault( csl::PatternWeights::PatternType( 1, 2 ), 1 );
	
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) == 1 ); // 1 ins inside the word
 	for( std::vector< Instruction >::const_iterator it = instructions.begin(); it != instructions.end(); ++it ) {
 	    it->print();std::wcout<<std::endl;
 	}

	CPPUNIT_ASSERT( instructions.size() == 3 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"u", L"xu", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 2 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 2 ).at( 0 ) == PosPattern( L"m", L"mx", 0 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) == 1 ); // 1 ins at beginning
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"xm", 0 ) );
	instructions.clear();




    }

} // namespace csl


#endif
