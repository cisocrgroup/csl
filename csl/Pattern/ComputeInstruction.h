#include "math.h"
#include<iostream>
#include <vector>
#include <algorithm>
#include<string>
#include<limits>
#include<string.h>
#include<csl/Global.h>
#include<csl/Pattern/Instruction.h>
#include<csl/Pattern/PatternWeights.h>

namespace csl {

/**
* @brief Computes the weighted levenshtein distance of two words 
*
* @author Nurhan Sayhan, Musa Ilker Hatipoglu, Ahmet Saglam 
*/

    class ComputeInstruction {
    public:
	
	/**
	 * @brief Constructor
	 */
	ComputeInstruction();

	/**
	 * @brief Destructor
	 */
	~ComputeInstruction();
	
	/**
	 * @brief Computes the weight levenshtein distance of two words
	 * @param wCorr The correct word 
	 * @param wErr The incorrect word 
	 * @param[out] instructions is a pointer to a vector of instructions, which is filled with all (equally) best ways to transform the strings.
	 */
	float computeInstruction(const std::wstring& wCorr, const std::wstring& wErr, std::vector< Instruction >* instruction = 0 );
	
	
	/**
	 * @brief Creates a connection to a LevenshteinWeights object
	 */
	void connectPatternWeights( PatternWeights const& levW );
	
	/**
	 * @brief Sets debug_ 
	 */
	void setDebug(bool d);
	
    private:
	bool debug_;

	class PatternTypeChain : public PatternWeights::PatternType {
	public:
	    PatternTypeChain( PatternWeights::PatternType const& patternType, PatternTypeChain* n ) : 
		PatternType( patternType ),
		next( n ) {
	    }

	    PatternTypeChain* next;
	};

	class MatrixItem {
	public:
	    MatrixItem() : patternTypes( 0 ) {}
	    ~MatrixItem() {
		// std::wcerr << "csl::ComputeInstruction::MatrixItem: destructor" << std::endl;
		reset();
	    }
	    void reset() {
		value = 0;
		removePatternTypes();
	    }

	    void removePatternTypes() {
		PatternTypeChain* next = 0;
		while( patternTypes ) {
		    next = patternTypes->next;
		    delete( patternTypes );
		    patternTypes = next;
		}
	    }

	    void addPatternType( PatternWeights::PatternType const& patternType ) {
		patternTypes = new PatternTypeChain( patternType, patternTypes );
	    }

	    float value;
	    PatternTypeChain* patternTypes;
	};

	
	void getInstructions( int x, int y, size_t instructionIndex );
	
	/** 
	 * @todo Find a good, dynamic solution here!
	 */
	MatrixItem matrix_[500][500];
	std::wstring wordCorr_;
	std::wstring wordErr_;
	const PatternWeights* patternWeights_;

	std::vector< Instruction >* instructions_;

	
    }; // class ComputeInstruction

} // ns csl


