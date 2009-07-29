#include "./ComputeInstruction.h"

namespace csl {
    ComputeInstruction::ComputeInstruction() : 
	patternWeights_( 0 ),
	instructions_( 0 ) {

	debug_ = false;

	/** 
	 * @todo Find a good, dynamic solution here!
	 */
	memset( matrix_, 0, 500 * 500 * sizeof( MatrixItem ) );

    }
	
    ComputeInstruction::~ComputeInstruction(){
    }

    void ComputeInstruction::connectPatternWeights( csl::PatternWeights const& patternWeights ){
	patternWeights_ = &patternWeights;
    }
    

    float ComputeInstruction::computeInstruction( const std::wstring& wCorr, 
						  const std::wstring& wErr, 
						  std::vector< Instruction >* instructions ) {
	
 	std::wcerr << "compare " << wCorr << ", " << wErr << std::endl;

	if( ! patternWeights_ ) {
	    throw exceptions::LogicalError( "csl::ComputeInstruction::ComputeInstruction: No patternWeights loaded." );
	    exit( 1 );
	}
	
	//levWeights_->printFreqPattern();
	//levWeights_->printFreqCorrect();
	//levWeights_->printPatternWeights();
	
	wordCorr_ = L" " + wCorr;
	wordErr_ = L" " + wErr;
	
	int matrixW = wordCorr_.length();
	int matrixH = wordErr_.length();
	
	//Set the values for the first row
	matrix_[0][0].value = 0;
	for(int i=1; i < matrixW; ++i ) {
	    float weight = patternWeights_->getWeight( Pattern( wordCorr_.substr( i, 1 ), L"" ) );
	    if( ( matrix_[i-1][0].value != PatternWeights::UNDEF ) && ( weight != PatternWeights::UNDEF ) ) {
		matrix_[0][i].value = matrix_[0][i-1].value + weight;
		matrix_[0][i].addPatternType( PatternWeights::PatternType( 1, 0 ) );
	    }
	    else {
		matrix_[0][i].value = PatternWeights::UNDEF;
		matrix_[0][i].removePatternTypes();
	    }
	}
	
	//Set the values for the first column
	for(int i=1; i < matrixH; ++i ) {
	    float weight = patternWeights_->getWeight( Pattern( wordErr_.substr( i, 1 ), L"" ) );
	    if( ( matrix_[i-1][0].value != PatternWeights::UNDEF) && ( weight != PatternWeights::UNDEF ) ) {
		matrix_[i][0].value = matrix_[i-1][0].value + weight;
		matrix_[i][0].addPatternType( PatternWeights::PatternType( 0, 1 ) );
	    }
	    else {
		matrix_[i][0].value = PatternWeights::UNDEF;
		matrix_[i][0].removePatternTypes();
	    }
	}

	
	// Set the values of the matrix.
	float del = 0;
	float add = 0;
	float sub = 0;
	float merge = 0;
	float split = 0;
	for( int y=1; y < matrixH; y++ ) {
	    for( int x = 1; x < matrixW; ++x ){
		// float minValue =  std::numeric_limits< float >::max();
		float minValue =  PatternWeights::UNDEF;

		// match, substitution
		if( matrix_[y-1][x-1].value == PatternWeights::UNDEF ) {
		    // do nothing
		}
		else if( wordCorr_.at(x) == wordErr_.at(y) ) {
		    sub = matrix_[y-1][x-1].value;
		    
		    if( ( minValue == PatternWeights::UNDEF ) || ( sub < minValue ) ) {
			minValue = sub;
			matrix_[y][x].removePatternTypes();
			matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
		    }
		    else if( sub == minValue) {
			matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
		    }
		}
		else {
		    sub = patternWeights_->getWeight( Pattern( wordCorr_.substr( x, 1 ), wordErr_.substr( y, 1 ) ) );
		    
		    if( ( sub != PatternWeights::UNDEF ) ) {
			sub += matrix_[y-1][x-1].value;
			
			if( ( minValue == PatternWeights::UNDEF ) || ( sub < minValue ) ) {
			    minValue = sub;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
			}
			else if( sub == minValue ) {
			    matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
			}
		    }
		}

		
		
		// Insert
		if( ( patternWeights_->getDefault( PatternWeights::PatternType( 0, 1 ) ) != PatternWeights::UNDEF ) && 
		    ( matrix_[y-1][x].value != PatternWeights::UNDEF ) ) {
		    add = matrix_[y-1][x].value+ 1;

		    if( ( minValue == PatternWeights::UNDEF ) || ( add < minValue ) ) {
			minValue = add;
			matrix_[y][x].removePatternTypes();
			matrix_[y][x].addPatternType( PatternWeights::PatternType( 0, 1 ) );
		    }
		    else if( add == minValue ) {
			matrix_[y][x].addPatternType( PatternWeights::PatternType( 0, 1 ) );
		    }
		}
		
		// Delete
		if( ( patternWeights_->getDefault( PatternWeights::PatternType( 1, 0 ) ) != PatternWeights::UNDEF ) &&
		    ( matrix_[y][x-1].value != -1 ) ) {
		    del = matrix_[y][x-1].value+1;
		    if( ( minValue == PatternWeights::UNDEF ) || ( del < minValue ) ) {
			minValue = del;
			matrix_[y][x].removePatternTypes();
			matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 0 ) );
		    }
		    else if(del == minValue) {
			matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 0 ) );
		    }
		}
		
		//std::wcout << "\"" << wordCorr_[x]<< " "<< wordErr_[y] << "\" | ";
		
		//Merge
		if( ( x >= 2 ) && ( matrix_[y-1][x-2].value != -1 ) ) {
		    merge = patternWeights_->getWeight( Pattern( wordCorr_.substr( x-1, 2 ), wordErr_.substr( y, 1 ) ) );
		    //std::wcout << "str : "<<  str << " | " << "merge : " << merge << " | ";
		    if(merge != PatternWeights::UNDEF ) {
			merge+=matrix_[y-1][x-2].value;

			if( ( minValue == PatternWeights::UNDEF ) || (merge < minValue ) ) {
			    minValue = merge;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternWeights::PatternType( 2, 1 ) );
			}
			else if(merge == minValue) {
			    matrix_[y][x].addPatternType( PatternWeights::PatternType( 2, 1 ) );
			}
		    }
		}

		//Split
		if( ( y >= 2 ) && ( matrix_[y-2][x-1].value != -1 ) ) {
		    std::wstring strLeft, strRight;
		    strLeft = strLeft + wordCorr_.at(x);
		    strRight = strRight + wordErr_.at(y-1) + wordErr_.at(y);
		    csl::Pattern pat(strLeft, strRight);
		    
		    split = patternWeights_->getWeight( Pattern( wordCorr_.substr( x, 1 ), wordErr_.substr( y-1, 2 ) ) );
		    //std::wcout << "str : "<< str << " | " << "split : " << split << " | ";
		    if( split  != PatternWeights::UNDEF ) {
			split += matrix_[y-2][x-1].value;
			

			if( ( minValue == PatternWeights::UNDEF ) || ( split < minValue ) ) {
			    minValue = split;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 2 ) );
			}
			else if(split == minValue) {
			    matrix_[y][x].addPatternType( PatternWeights::PatternType( 1, 2 ) );
			}
		    }
		} // split
				
		
		matrix_[y][x].value = minValue;				
		
		
	    } // for( y = 1 .. matrixW )

	    //std::wcout << "-------------------------------" <<std::endl;
			
	} // for(y = 1 .. matrixH)

	if( debug_ ) {
	    //std::wcout << "wCorr (wstring&): " << "\'"<< wCorr << "\'"<< std::endl;
	    //std::wcout << "wErr (wstring&): " << "\'"<< wErr << "\'"<< std::endl;
	    std::wcout << "wordCorr_ (wstring): " << "\'" << wordCorr_ << "\'"<< std::endl;
	    std::wcout << "wordErr_ (wstring): " << "\'" << wordErr_ << "\'"<< std::endl;
	    std::wcout << "matrixW (wordCorr_.length()): " << matrixW << std::endl;
	    std::wcout << "matrixH (wordErr_.length()): " << matrixH << std::endl;
	    
	    std::wcout << std::endl << "  |" ;
	    for(int x=0; x<matrixW; x++){
		std::wcout << " " << wordCorr_[x] << " |";
	    }
	    std::wcout << std::endl;
	    
	    for(int y=0; y<matrixH; y++){
		std::wcout << wordErr_[y] << " |";
		for(int x = 0; x < matrixW; x++){
		    std::wcout << " " << matrix_[y][x].value;
		    if( matrix_[y][x].patternTypes ) {
			std::wcout <<  "[" 
				   << matrix_[y][x].patternTypes->first << ","
				   << matrix_[y][x].patternTypes->second << "]";
		    }
		    else std::wcout << "[   ]";
		    
		    std::wcout << " |";
		    
		}
		std::wcout << std::endl;
	    }
	    std::wcout << std::endl;
	    
	    int x = matrixW - 1;
	    int y = matrixH - 1;
	    PatternTypeChain* patternType = matrix_[y][x].patternTypes;
	    while( patternType ) {
		if( ( patternType->first == 1 ) && ( patternType->second == 1 ) && ( wordCorr_.at( x ) == wordErr_.at( y ) ) ) {
		    x -= 1;
		    y -= 1;
		    // do nothing: match
		}
		else {
		    std::wcout<< wordCorr_.substr( x-patternType->first + 1, patternType->first ) << "->" << wordErr_.substr( y - patternType->second + 1, patternType->second ) << " at pos " << x - patternType->first  << std::endl;
		    x -= patternType->first;
		    y -= patternType->second;
		}
		
		patternType = matrix_[y][x].patternTypes;
	    }
	    
	} // if debug_

	float doReturn = matrix_[matrixH-1][matrixW-1].value;
	
	instructions_ = instructions;
	
	if( instructions_ ) { 
	    if( ! instructions_->empty() ) {
		throw csl::exceptions::cslException( "csl::ComputeInstruction::computeInstruction: answer object 'instructions' not empty." );
	    }

	    if( doReturn != PatternWeights::UNDEF ) {
		instructions_->push_back( Instruction() ); // create a first instruction to work on
		getInstructions( matrixW-1, matrixH-1, 0 );
	    }
	}


	for( int y=0; y < matrixH; y++ ) {
	    for( int x = 0; x < matrixW; ++x ){
		matrix_[y][x].reset();
	    }
	}

	return doReturn;

    } // function computeInstruction

    /**
     * @brief recursive method to find all best paths through the levenshtein-matrix and store the resulting csl::Instruction s
     */
    void ComputeInstruction::getInstructions( int x, int y, size_t instructionIndex ) {
	if( ( x== 0 && y == 0 ) ) {
	    // no recursive calls, we're at the end. Reverse the complete instruction
	    std::reverse( instructions_->at( instructionIndex ).begin(), instructions_->at( instructionIndex ).end() );
	}
	else {
	    PatternTypeChain* patternType = matrix_[y][x].patternTypes;
	    
	    if( patternType == 0 ) {
		//return;
		throw std::runtime_error( "patternType can not be 0" );
	    }
	    
	    size_t countPatternTypes = 0;
	    while( patternType ) {
		size_t currentInstructionIndex = 0;
		
		if( patternType->next == 0  ) { // no need to clone for the last patternType
		    currentInstructionIndex = instructionIndex; 
		    //std::wcerr<<"Do not clone: work on instr["<<instructionIndex<<"]"<<std::endl;
		    //std::wcerr<<"instr.size() is "<<instructions_->size()<<std::endl;
		}
		else { // clone
		    instructions_->push_back( instructions_->at( instructionIndex ) ); // clone the instruction as built so far
		    currentInstructionIndex = instructions_->size() - 1;
		    //std::wcerr<<"Clone instr["<<instructionIndex<<"] to instr["<<currentInstructionIndex<<"]"<<std::endl;
		}

		// match: continue
		if( ( patternType->first == 1 ) && ( patternType->second == 1 ) && ( wordCorr_.at( x ) == wordErr_.at( y ) ) ) {
		    getInstructions( x - 1, y - 1, 
				     currentInstructionIndex ); // recursive call
		}
		else {
		
		//std::wcout<< wordCorr_.substr( x-patternType->first + 1, patternType->first ) << "->" << wordErr_.substr( y - patternType->second + 1, patternType->second ) << " at pos " << x - patternType->first  << std::endl;
		
		    
		    instructions_->at( currentInstructionIndex ).push_back( 
			PosPattern( 
			    wordCorr_.substr( x-patternType->first + 1, patternType->first ),
			    wordErr_.substr( y - patternType->second + 1, patternType->second ),
			    x - patternType->first ) 
			);
		    
		    getInstructions(  x - patternType->first, y - patternType->second, 
				      currentInstructionIndex ); // recursive call
		    
		    
		}
		
		patternType = patternType->next;
		++countPatternTypes;

	    }
	    
	} // if not at the left upper corner od the matrix
    } // getInstructions()
    
    
    
    
    void ComputeInstruction::setDebug( bool d ) {
	debug_ = d;
    }
    
} // eon

