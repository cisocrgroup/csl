#include "./ComputeInstruction.h"

namespace csl {
    ComputeInstruction::ComputeInstruction() : 
	patternWeights_( 0 ) {
	debug_ = false;
    }
	
    ComputeInstruction::~ComputeInstruction(){
    }

    void ComputeInstruction::connectPatternWeights( csl::PatternWeights* patternWeights ){
	patternWeights_ = patternWeights;
    }
    
    float ComputeInstruction::computeInstruction( const std::wstring& wCorr, 
						  const std::wstring& wErr, 
						  std::vector< Instruction >* instruction ) {
	if( ! patternWeights_ ) {
	    std::wcerr<<"THROW EXCEPTION HERE"<<std::endl;
	    exit( 1 );
	}
	
	//levWeights_->printFreqPattern();
	//levWeights_->printFreqCorrect();
	//levWeights_->printPatternWeights();
	
	std::wstring wordCorr = L" " + wCorr;
	std::wstring wordErr = L" " + wErr;
	
	int matrixW = wordCorr.length();
	int matrixH = wordErr.length();

	/** 
	 * @todo Find a good, dynamic solution here!
	 */
//	float matrix[matrixH][matrixW];
	MatrixItem matrix[500][500];
	memset( matrix, 0, 500 * 500 * sizeof( MatrixItem ) );
		
	//Set the values for the first row
	for(int i=1; i < matrixW; i++){
	    matrix[0][i].value = i;
	    matrix[0][i].addPatternType( PatternWeights::PatternType( 1, 0 ) );
	}

	//Set the values for the first column
	for(int i=1; i < matrixH; i++){
	    matrix[i][0].value = i;
	    matrix[i][0].addPatternType( PatternWeights::PatternType( 0, 1 ) );
	}
	
	//Set the values of the matrix.
	float del = 0;
	float add = 0;
	float sub = 0;
	float merge = 0;
	float split = 0;
	float minValue =  std::numeric_limits< float >::max();
	for( int y=1; y < matrixH; y++ ) {
	    for( int x = 1; x < matrixW; ++x ){
		
		// Insert
		if( ( patternWeights_->getDefault( PatternWeights::PatternType( 0, 1 ) ) != PatternWeights::UNDEF ) && 
		    ( matrix[y-1][x].value != -1 ) ) {
		    add = matrix[y-1][x].value+ 1;
		    minValue = add;
		    matrix[y][x].addPatternType( PatternWeights::PatternType( 0, 1 ) );
		}
		
		// Delete
		if( ( patternWeights_->getDefault( PatternWeights::PatternType( 1, 0 ) ) != PatternWeights::UNDEF ) &&
		    ( matrix[y][x-1].value != -1 ) ) {
		    del = matrix[y][x-1].value+1;
		    if(del == minValue) {
			matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 0 ) );
		    }
		    else if(del < minValue) {
			minValue = del;
			matrix[y][x].removePatternTypes();
			matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 0 ) );
		    }
		}
		
		//std::wcout << "\"" << wordCorr[x]<< " "<< wordErr[y] << "\" | ";
		
		//Merge
		if( ( x >= 2 ) && ( matrix[y-1][x-2].value != -1 ) ) {
		    merge = patternWeights_->getWeight( Pattern( wordCorr.substr( x-1, 2 ), wordErr.substr( y, 1 ) ) );
		    //std::wcout << "str : "<<  str << " | " << "merge : " << merge << " | ";
		    if(merge != PatternWeights::UNDEF ) {
			merge+=matrix[y-1][x-2].value;

			if( merge == minValue) {
			    matrix[y][x].addPatternType( PatternWeights::PatternType( 2, 1 ) );
			}
			else if(merge < minValue) {
			    minValue = merge;
			    matrix[y][x].removePatternTypes();
			    matrix[y][x].addPatternType( PatternWeights::PatternType( 2, 1 ) );
			}
		    }
		}

		//Split
		if( ( y >= 2 ) && ( matrix[y-2][x-1].value != -1 ) ) {
		    std::wstring strLeft, strRight;
		    strLeft = strLeft + wordCorr.at(x);
		    strRight = strRight + wordErr.at(y-1) + wordErr.at(y);
		    csl::Pattern pat(strLeft, strRight);
		    
		    split = patternWeights_->getWeight( Pattern( wordCorr.substr( x, 1 ), wordErr.substr( y-1, 2 ) ) );
		    //std::wcout << "str : "<< str << " | " << "split : " << split << " | ";
		    if( split  != PatternWeights::UNDEF ) {
			split += matrix[y-2][x-1].value;


			if( split == minValue) {
			    matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 2 ) );
			}
			else if(split < minValue) {
			    minValue = split;
			    matrix[y][x].removePatternTypes();
			    matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 2 ) );
			}
		    }
		}
				
		// match, substitution
		if( wordCorr.at(x) == wordErr.at(y) ) {
		    sub = matrix[y-1][x-1].value;
		    if( sub == minValue) {
			matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
		    }
		    else if(sub < minValue) {
			minValue = sub;
			matrix[y][x].removePatternTypes();
			matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
		    }
		}
		else {
		    sub = patternWeights_->getWeight( Pattern( wordCorr.substr( x, 1 ), wordErr.substr( y, 1 ) ) );
		    
		    if( sub != PatternWeights::UNDEF ) {
			sub += matrix[y-1][x-1].value;
		    }
		    if( sub == minValue) {
			matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
		    }
		    else if(sub < minValue) {
			minValue = sub;
			matrix[y][x].removePatternTypes();
			matrix[y][x].addPatternType( PatternWeights::PatternType( 1, 1 ) );
		    }
		}
		
		matrix[y][x].value = minValue;				
		
		
		/*std::wcout << "sub : " << sub << " | ";
		  std::wcout << "add : " << add << " | ";
		  std::wcout << "del : " << del << " | " << std::endl;*/
	    }
	    //std::wcout << "-------------------------------" <<std::endl;
			
	}

	if( debug_ ) {
	    std::wcout << "wCorr (wstring&): " << "\'"<< wCorr << "\'"<< std::endl;
	    std::wcout << "wErr (wstring&): " << "\'"<< wErr << "\'"<< std::endl;
	    std::wcout << "wordCorr (wstring): " << "\'" << wordCorr << "\'"<< std::endl;
	    std::wcout << "wordErr (wstring): " << "\'" << wordErr << "\'"<< std::endl;
	    std::wcout << "matrixW (wordCorr.length()): " << matrixW << std::endl;
	    std::wcout << "matrixH (wordErr.length()): " << matrixH << std::endl;
	    
	    std::wcout << std::endl << "  |" ;
	    for(int x=0; x<matrixW; x++){
		std::wcout << " " << wordCorr[x] << " |";
	    }
	    std::wcout << std::endl;
	    
	    for(int y=0; y<matrixH; y++){
		std::wcout << wordErr[y] << " |";
		for(int x=0; x<matrixW; x++){
		    std::wcout << " " << matrix[y][x].value;
		    if( matrix[y][x].patternTypes ) {
			std::wcout <<  "[" 
				   << matrix[y][x].patternTypes->first << ","
				   << matrix[y][x].patternTypes->second << "]";
		    }
		    else std::wcout << "[   ]";

		    std::wcout << " |";

		}
		std::wcout << std::endl;
	    }
	    std::wcout << std::endl;

	    int x = matrixW - 1;
	    int y = matrixH - 1;
	    PatternTypeChain* patternType = matrix[y][x].patternTypes;
	    while( patternType ) {
		if( ( patternType->first == 1 ) && ( patternType->second == 1 ) && ( wordCorr.at( x ) == wordErr.at( y ) ) ) {
		    x -= 1;
		    y -= 1;
		    // do nothing: match
		}
		else {
		    std::wcout<< wordCorr.substr( x-patternType->first + 1, patternType->first ) << "->" << wordErr.substr( y - patternType->second + 1, patternType->second ) << " at pos " << x - patternType->second  << std::endl;
		    x -= patternType->first;
		    y -= patternType->second;
		}
		
		patternType = matrix[y][x].patternTypes;
	    }
	    
	}
	return matrix[matrixH-1][matrixW-1].value;
    }

    void ComputeInstruction::setDebug(bool d){
	debug_ = d;
    }
    

} // eon

