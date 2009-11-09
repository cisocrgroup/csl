#ifndef CSL_TEST_PATTERN_H
#define CSL_TEST_PATTERN_H CSL_TEST_PATTERN_H

#include "../Pattern.h"
#include "../PatternSet.h"
#include "../PosPattern.h"
#include "../Interpretation.h"
#include "../PatternProbabilities.h"
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
	CPPUNIT_TEST( testInstruction );
	CPPUNIT_TEST( testInterpretation );
	CPPUNIT_TEST( testPatternProbabilities );
	CPPUNIT_TEST( testComputeInstruction );
	CPPUNIT_TEST_SUITE_END();
    public:

	void testPattern();
	void testPatternSet();
	void testPosPattern();
	void testInstruction();
	void testInterpretation();
	void testPatternProbabilities();
	void testComputeInstruction();

	void run();
    private:
	
    }; // class TestPattern

    CPPUNIT_TEST_SUITE_REGISTRATION( TestPattern );

    
    void TestPattern::run() {
 	testPattern();
 	testPatternSet();
 	testPosPattern();
	testInstruction();
	testInterpretation();
 	testPatternProbabilities();
 	testComputeInstruction();
    }

    /**
     * test the basic methods of class Pattern.
     */
    void TestPattern::testPattern() {
	// test constructor and the getters. 
	Pattern p1( L"left", L"right" );
	CPPUNIT_ASSERT( p1.getLeft() == L"left" );
	CPPUNIT_ASSERT( p1.getRight() == L"right" );
    }

    void TestPattern::testPatternSet() {
	PatternSet pSet;
	//pSet.loadPatterns( "./test.patterns.txt" );
    }

    void TestPattern::testPosPattern() {
	
	// construct PosPattern from existing Pattern
	PosPattern pp1( L"left", L"right", 3 );
	CPPUNIT_ASSERT( pp1.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp1.getRight() == L"right" );
	CPPUNIT_ASSERT( pp1.getPosition() == 3 );

	std::wstring str = pp1.toString();
	CPPUNIT_ASSERT( str == L"(left_right,3)" );

	PosPattern pp2;
	pp2.parseFromString( str );
	CPPUNIT_ASSERT( pp2.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp2.getRight() == L"right" );
	CPPUNIT_ASSERT( pp2.getPosition() == 3 );
	
	// if posPattern is not at the beginning of the str
	str = L"bliblablu(left_right,3)bla";
    
	PosPattern pp3;
	size_t end_of_pp = pp3.parseFromString( str, 9 );
	CPPUNIT_ASSERT( pp3.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp3.getRight() == L"right" );
	CPPUNIT_ASSERT( pp3.getPosition() == 3 );
	CPPUNIT_ASSERT( end_of_pp == 23 );
	
	// in a realistic Instruction
	str = L"[(left_right,3)(le_ri,42)]";
    
	PosPattern pp4;
	end_of_pp = pp4.parseFromString( str, 1 );
	CPPUNIT_ASSERT( pp4.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp4.getRight() == L"right" );
	CPPUNIT_ASSERT( pp4.getPosition() == 3 );
	CPPUNIT_ASSERT( end_of_pp == 15 );
	
	PosPattern pp5;
	end_of_pp = pp5.parseFromString( str, 15 );
	CPPUNIT_ASSERT( pp5.getLeft() == L"le" );
	CPPUNIT_ASSERT( pp5.getRight() == L"ri" );
	CPPUNIT_ASSERT( pp5.getPosition() == 42 );
	CPPUNIT_ASSERT( end_of_pp == 25 );
	

    }

    void TestPattern::testInstruction() {
	Instruction instr;
	instr.push_back( PosPattern( L"left", L"right", 3 ) );
	instr.push_back( PosPattern( L"le", L"ri", 42 ) );

	CPPUNIT_ASSERT( instr.size() == 2 );
	CPPUNIT_ASSERT( instr.at( 0 ).getLeft() == L"left" );
	CPPUNIT_ASSERT( instr.at( 0 ).getRight() == L"right" );
	CPPUNIT_ASSERT( instr.at( 0 ).getPosition() == 3 );
	CPPUNIT_ASSERT( instr.at( 1 ).getLeft() == L"le" );
	CPPUNIT_ASSERT( instr.at( 1 ).getRight() == L"ri" );
	CPPUNIT_ASSERT( instr.at( 1 ).getPosition() == 42 );

	std::wstring str = instr.toString();
	std::wcout << str << std::endl;
	CPPUNIT_ASSERT( str == L"[(left_right,3)(le_ri,42)]" );

	Instruction instr2;
	instr2.parseFromString( str, 0 );
	CPPUNIT_ASSERT( instr2.size() == 2 );
	CPPUNIT_ASSERT( instr2.at( 0 ).getLeft() == L"left" );
	CPPUNIT_ASSERT( instr2.at( 0 ).getRight() == L"right" );
	CPPUNIT_ASSERT( instr2.at( 0 ).getPosition() == 3 );
	CPPUNIT_ASSERT( instr2.at( 1 ).getLeft() == L"le" );
	CPPUNIT_ASSERT( instr2.at( 1 ).getRight() == L"ri" );
	CPPUNIT_ASSERT( instr2.at( 1 ).getPosition() == 42 );
	
    }

    void TestPattern::testInterpretation() {
	Interpretation interp;
	interp.setWord( L"theil" );
	interp.setBaseWord( L"teil" );
	interp.getHistInstruction().push_back( csl::PosPattern( L"t", L"th", 0 ) );
	interp.setLevDistance( 1 );

	std::wstring str = interp.toString();
	std::wcout << str << std::endl;
	CPPUNIT_ASSERT( str == L"theil:teil+[(t_th,0)],dist=1" );

	interp.clear();
	interp.parseFromString( str, 0 );
	CPPUNIT_ASSERT( interp.getWord() == L"theil" );
	CPPUNIT_ASSERT( interp.getBaseWord() == L"teil" );
	CPPUNIT_ASSERT( interp.getBaseWordScore() == -1 );
	CPPUNIT_ASSERT( interp.getLevDistance() == 1 );
	CPPUNIT_ASSERT( interp.getHistInstruction().size() == 1 );
	CPPUNIT_ASSERT( interp.getHistInstruction().at( 0 ).getLeft() == L"t" );
	CPPUNIT_ASSERT( interp.getHistInstruction().at( 0 ).getRight() == L"th" );
	CPPUNIT_ASSERT( interp.getHistInstruction().at( 0 ).getPosition() == 0 );
    }

    void TestPattern::testPatternProbabilities() {
	PatternProbabilities pp;
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == PatternProbabilities::UNDEF );
	pp.setWeight( Pattern( L"t", L"th" ), 0.35 );
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) );
	
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), 1.3 );
	
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) ); // as before
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"x", L"yz" ) ) == static_cast< float >( 1.3 ) ); // default value
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"xy", L"z" ) ) == PatternProbabilities::UNDEF ); // as before

	pp.reset();
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == PatternProbabilities::UNDEF );
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"x", L"yz" ) ) == PatternProbabilities::UNDEF );
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"xy", L"z" ) ) == PatternProbabilities::UNDEF );


    }


    void TestPattern::testComputeInstruction() {
	ComputeInstruction ci;
	ci.setDebug( 1 );
	PatternProbabilities pp;
	ci.connectPatternProbabilities( pp );

	std::vector< Instruction > instructions;
	

	// all operations are tested separately


	// sub
	pp.reset();
	pp.setWeight( Pattern( L"x", L"y" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"abxcd", L"abycd", &instructions ) ); // 1 sub
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"x", L"y", 2 ) );


	// insert
	pp.reset();
	instructions.clear();
	pp.setWeight( Pattern( L"", L"x" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );

	// delete
	pp.reset();
	instructions.clear();
	pp.setWeight( Pattern( L"x", L"" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mxuh", L"muh", &instructions ) ); // 1 del
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"x", L"", 1 ) );

	// merge
	pp.reset();
	instructions.clear();
	pp.setWeight( Pattern( L"xy", L"z" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mxyuh", L"mzuh", &instructions ) ); // 1 del
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"xy", L"z", 1 ) );

	// split
	pp.reset();
	instructions.clear();
	pp.setWeight( Pattern( L"x", L"yz" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mxuh", L"myzuh", &instructions ) ); // 1 del
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"x", L"yz", 1 ) );

	
	pp.reset();
	pp.setDefault( PatternProbabilities::PatternType( 0, 1 ), 0.00001 ); // standard ins
	pp.setDefault( PatternProbabilities::PatternType( 1, 0 ), 0.00001 ); // standard del
	pp.setDefault( PatternProbabilities::PatternType( 1, 1 ), 0.00001 ); // standard sub

	pp.setDefault( PatternProbabilities::PatternType( 2, 1 ), 0.00001 ); // standard merge
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), 0.00001 ); // standard split
	
	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"abcde", L"abxcde", &instructions ) );

	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"abcde", L"axcdy", &instructions ) ); // 2 sub


	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins
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


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // 1 ins at beginning of word

	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"xm", 0 ) );
	instructions.clear();


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxh", &instructions ) ); // 1 sub
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"u", L"x", 1 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xuh", &instructions ) ); // 1 sub at beginning of word
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"x", 0 ) );
	instructions.clear();

	// deactivate the merges and splits again
	pp.setDefault( PatternProbabilities::PatternType( 2, 1 ), PatternProbabilities::UNDEF ); // standard merge
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), PatternProbabilities::UNDEF ); // standard split

	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"nrilk", &instructions ) ); // 1 ins, 1 sub
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"m", L"r", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 1 ) == PosPattern( L"", L"r", 1 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"murnau", L"mumau", &instructions )  ); // 1 del, 1 sub
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"r", L"", 2 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"n", L"m", 3 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"r", L"m", 2 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 1 ) == PosPattern( L"n", L"", 3 ) );
	instructions.clear();

	pp.setWeight( Pattern( L"m", L"rn" ), 0.35 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"rnilk", &instructions ) ); // 1 split
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"rn", 0 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"rnilkx", &instructions ) ); // 1 split, 1 insertion
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"rn", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"", L"x", 4 ) );
	instructions.clear();

	pp.setWeight( Pattern( L"m", L"n" ), 0.54 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"milkman", L"nilkrnan", &instructions ) ); // 1 sub 0.54, 1 split 0.35
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"m", L"rn", 4 ) );


	// this is a setup that is used for the Profiler, for instance
	/// obviously the merges and splits give additional trouble
	pp.reset();
	pp.setDefault( PatternProbabilities::PatternType( 1, 1 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 1, 0 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 0, 1 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 2, 1 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), 0.00001 );
	
	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins inside the word

	std::wcout << "size is "<< instructions.size() << std::endl;

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

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // 1 ins at beginning
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"xm", 0 ) );
	instructions.clear();

	////// smartMerge //////
	// now those annoying pseudo-merges should disappear
	pp.setSmartMerge( true );

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins inside the word
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );
	instructions.clear();
	
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // 1 ins at beginning
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	instructions.clear();

	
	// What about spaces in words? Here's a real-world example
	CPPUNIT_ASSERT( ci.computeInstruction( L"eure churfuerstliche", L"fuerstltche", &instructions ) );
	instructions.clear();
	
	
	// use only sub as default operations
	pp.clear();
	pp.setDefault( PatternProbabilities::PatternType( 1, 1 ), 1 );
	
	CPPUNIT_ASSERT( ! ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // ins (not allowed)
	CPPUNIT_ASSERT( instructions.empty() );

	instructions.clear();
	
	CPPUNIT_ASSERT( ! ci.computeInstruction( L"galich", L"lich", &instructions )  ); // del (not allowed)
	CPPUNIT_ASSERT( instructions.empty() );
	
	instructions.clear();

	CPPUNIT_ASSERT( ! ci.computeInstruction( L"glich", L"galich", &instructions ) ); // ins (not allowed)
	CPPUNIT_ASSERT( instructions.empty() );


    }

} // namespace csl


#endif
