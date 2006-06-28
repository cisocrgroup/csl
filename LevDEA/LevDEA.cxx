#ifndef LEVDEA_CXX
#define LEVDEA_CXX LEVDEA_CXX

#include "./LevDEA.h"


namespace csl {

    /*
      This practice of sharing the tables here is very very dangerous!
      The first instance whose destructor is called will delete the tables
      and leave the other instances with pointers to noone's land!!!
      This only works as long as all instances die at the same time
    */
    bits32 LevDEA::tabsLoaded = 0;
    table_cell** LevDEA::tabs = new table_cell*[4];
    int** LevDEA::fins = new int*[4];
    int* LevDEA::coresetss = new int[4];

    LevDEA::LevDEA(const Alphabet& init_alph,int init_k) : alph(init_alph), k(init_k) {
	pattern_ = (uchar*) malloc(Global::lengthOfWord * sizeof(uchar));
	charvec = (bits64*) malloc(alph.size() * sizeof(bits64));
	initDistance();
    }

    LevDEA::~LevDEA() {
//	delete(tab);
//	delete(fin);
    }


// this is an extended version of walk.
// I now use a much more compact version as inline function
// LevDEA::Pos LevDEA::walk(const Pos& p,int c) const {
//     int bitvec = calc_k_charvec(charvec[c],p.pattern_pos());
//     table_cell& cell = table(bitvec,p.position());
//     int new_pos = cell.target;
//     int move_pattern = cell.move_pattern;
//     return Pos(new_pos, p.pattern_pos()+move_pattern);
// }

    LevDEA::Pos LevDEA::walk(const Pos& p,int c) const {
	table_cell& cell = table(calc_k_charvec(charvec[c],p.pattern_pos()),p.position());
	return Pos(cell.target, p.pattern_pos() + cell.move_pattern);
    }



    void LevDEA::initDistance() {
	// How ugly!!!
	// How ugly!!!
	char* table_file = "./LevDEA/lev0data";
	if(k==0) {
	    table_file = "./LevDEA/lev0data";
	}
	else if(k==1) {
	    table_file = "./LevDEA/lev1data";
	}
	else if(k==2) {
	    table_file = "./LevDEA/lev2data";
	}
	else if(k==3) {
	    table_file = "./LevDEA/lev3data";
	}

	/*
	  In case of k==1, z2k1 would be 111  (==7)
	  z2k2 would be 1111 (==15)
	*/
	z2k1 = 1ll; z2k1 <<= 2*k+1; z2k1--; // a sequence of 2k+1 1-values
	z2k2 = 1ll; z2k2 <<= 2*k+2; z2k2--; // a sequence of 2k+2 1-values

	if((tabsLoaded & (1<<k)) == 0) { // have the tables for k already been loaded???
	    FILE* table_handle = fopen(table_file,"r");

	    if(!table_handle) {
		std::cerr<<"Could not read "<<table_file<<std::endl;
		exit(1);
	    }
	    fscanf(table_handle,"%d\n",&k);
	    fscanf(table_handle,"%d\n",&(coresetss[k]));
	    coresets = coresetss[k];

	    tabs[k] = new table_cell[z2k2*coresets];
	    fins[k] = (int*)malloc((2*k+1)*coresets* sizeof(int));
	    tab = tabs[k];
	    fin = fins[k];

	    int row,col;
	    char c;

	    for (row=0; row<z2k2; row++) {
		for (col=0; col<coresets; ++col) {
		    fscanf(table_handle,"%d,%d,",&(table(row,col).target),&(table(row,col).move_pattern));
		}
		fscanf(table_handle,"%c",&c);
	    }

	    for (row=0; row<2*k+1; row++) {
		for (col=0; col<coresets; ++col) {
		    fscanf(table_handle,"%d,",&fin[coresets*row+col]);
		}
		fscanf(table_handle,"%c",&c);
	    }
	    fclose(table_handle);
	    tabsLoaded = tabsLoaded | (1<<k);
	}
	else {
	    // tables were loaded some time before. simply assign the pointers.
	    tab = tabs[k];
	    fin = fins[k];
	    coresets = coresetss[k];
	}
    }

    void LevDEA::loadPattern(const uchar* p) {
	strcpy((char*)pattern_,(char*)p);
	patLength = strlen((char*)pattern_);
	calcCharvec();
    }

    void LevDEA::calcCharvec() {
	bits64 c;
	int i;
	memset(charvec, 0, alph.size()*sizeof(bits64));
	for (c=z10, i=0; i<patLength; i++, c>>=1) {
	    charvec[alph.code(pattern_[i])] |= c;
	}
    }

    int LevDEA::calc_k_charvec(const bits64& chv, int i) const {
	bits64 r;
	// after the next line, the bits i,i+1,i+2 of chv are the lowest bits of r. All other bits of r are 0
	r = (chv >> (64-(2*k+1+i))) & z2k1; 
	if (patLength-i < 2*k+1) // the last few chars of the word
	    r = ((r >> (2*k+1-(patLength-i))) | (zff << ((patLength-i)+1))) & z2k2;
	return (int) r;
    }

    void LevDEA::printTable() const {
	int row,col;
	for (row=0; row<z2k2; row++) {
	    printf("%d\t",row);
	    for (col=0; col<coresets; ++col) {
		printf("(%d,%d)\t",table(row,col).target,table(row,col).move_pattern);
	    }
	    printf("\n");
	}
    }

    void LevDEA::printCharvec() const {
	int c;
	std::cout<<"-------------"<<std::endl;
	for(c=1;c<=alph.size();++c) {
	    if(charvec[c]) {
		std::cout<<c<<std::endl;
		printBits(charvec[c]);
	    }
	}
	std::cout<<"-------------"<<std::endl;
    }

    void LevDEA::printBits(const bits64& n) const {
	int i;
	for(i=63;i>=0;--i) {
	    if((i%10)==0) printf("%d",i/10);
	    else if((i%10)==5) printf("|");
	    else printf(" ");
	}
	printf("\n");
	for(i=63;i>=0;--i) {
	    printf("%i", (int)(1 & (n>>i)));
	}
	printf("\n");
    
    }

} // eon
#endif
