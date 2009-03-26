#include "math.h"
#include<iostream>
#include <vector>
#include <algorithm>
#include<string>
#include<string.h>
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
	 * @param debug Parameter for the debug-output. The output contains the values of the variables 
	 * and the levenshtein distance Matrix.  
	 */
	float computeInstruction(const std::wstring& wCorr, const std::wstring& wErr);
	
	
	/**
	 * @brief Creates a connection to a LevenshteinWeights object
	 */
	void connectPatternWeights( PatternWeights* levW);
	
	/**
	 * @brief Sets debug_ 
	 */
	void setDebug(bool d);
	
    private:
	bool debug_;
	const PatternWeights* patternWeights_;
	
    }; // class ComputeInstruction

} // ns csl


