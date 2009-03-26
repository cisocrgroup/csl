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
    
    float ComputeInstruction::computeInstruction( const std::wstring& wCorr, const std::wstring& wErr ) {
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
	float matrix[500][500];
	memset( matrix, 0, 500 * 500 * sizeof( float) );
		
	//Set the values for the first row
	for(int i=0; i < matrixW; i++){
	    matrix[0][i]=i;
	}

	//Set the values for the first column
	for(int i=0; i < matrixH; i++){
	    matrix[i][0]=i;
	}
		
	//Set the values of the matrix.
	float del = 0;
	float add = 0;
	float sub = 0;
	float merge = 0;
	float split = 0;
	float minValue = 0;
	for(int y=1; y<matrixH; y++){
	    for(int x=1; x<matrixW; x++){
				
		// Insert
		if( patternWeights_->getDefault( std::make_pair( 0, 1 ) ) != PatternWeights::UNDEF ) {
		    add = matrix[y-1][x]+ 1;
		    minValue = add;
		}
				
		//Delete
		if( patternWeights_->getDefault( std::make_pair( 1, 0 ) ) != PatternWeights::UNDEF ) {
		    del = matrix[y][x-1]+1;
		    if(del<minValue) minValue = del;
		}
			
		//std::wcout << "\"" << wordCorr[x]<< " "<< wordErr[y] << "\" | ";
				
		//Merge
		if( x >= 2 ) {
		    merge = patternWeights_->getWeight( Pattern( wordCorr.substr( x-1, 2 ), wordErr.substr( y, 1 ) ) );
		    //std::wcout << "str : "<<  str << " | " << "merge : " << merge << " | ";
		    if(merge != PatternWeights::UNDEF ) {
			merge+=matrix[y-1][x-2];
			if(merge<minValue) minValue = merge;
		    }
		}

		//Split
		if( y >= 2 ) {
		    std::wstring strLeft, strRight;
		    strLeft = strLeft + wordCorr.at(x);
		    strRight = strRight + wordErr.at(y-1) + wordErr.at(y);
		    csl::Pattern pat(strLeft, strRight);
		    
		    split = patternWeights_->getWeight( Pattern( wordCorr.substr( x, 1 ), wordErr.substr( y-1, 2 ) ) );
		    //std::wcout << "str : "<< str << " | " << "split : " << split << " | ";
		    if( split  != PatternWeights::UNDEF ) {
			split += matrix[y-2][x-1];
			if( split < minValue ) minValue = split;
		    }
		}
				
		// match, substitution
		if( wordCorr.at(x) == wordErr.at(y) ) {
		    sub = matrix[y-1][x-1];
		    if( sub < minValue ) minValue = sub;
		}
		else {
		    sub = patternWeights_->getWeight( Pattern( wordCorr.substr( x, 1 ), wordErr.substr( y, 1 ) ) );
		    
		    if( sub != PatternWeights::UNDEF ) {
			sub += matrix[y-1][x-1];
			if(sub<minValue) minValue = sub;
		    }
		}
		
		matrix[y][x]=minValue;				
		
		
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
		    std::wcout << " " << matrix[y][x] << " |";
		}
		std::wcout << std::endl;
	    }
	    std::wcout << std::endl;
	}
	return matrix[matrixH-1][matrixW-1];
    }

    void ComputeInstruction::setDebug(bool d){
	debug_ = d;
    }
    

} // eon

