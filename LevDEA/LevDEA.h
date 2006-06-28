#ifndef LEVDEA_H
#define LEVDEA_H LEVDEA_H

#include <fstream>
#include <iostream>
#include <stdio.h>

#include "../Alphabet/Alphabet.h"
#include "../Global.h"

namespace csl {

    struct table_cell {
	int target;
	int move_pattern;
    };



    class LevDEA {
    private:
	/// holds the current pattern
	uchar* pattern_;
    
	/// holds the pattern length
	int patLength;

	const Alphabet& alph;

	/// tab holds the transition table of the automaton 
	table_cell* tab;

	// easy access to the table
	inline table_cell& table(const int row,const int col) const {return tab[coresets*(row)+(col)];}
	inline int fin_table(int row,int col) const {
	    //      std::cerr<<row<<","<<col<<std::endl;
	    return fin[coresets*(row)+(col)];
	}


	/// fin holds the information about final states (??!)
	int *fin;
	/// k is the max levenshtein-distance
	int k;
	/// coresets is the number of states in the automaton ???
	int coresets;
    
	bits64* charvec;
	void calcCharvec();
	int calc_k_charvec(const bits64& chv, int i) const;

	// some bitvectors
	int z2k1;
	int z2k2;

	static bits32 tabsLoaded;
	static table_cell** tabs;
	static int** fins;
	static int* coresetss;

	static const bits64 zff = 0xffffffffffffffffll; // that's 64 '1's
	static const bits64 z10 = 1ll << 63; // that's a '1' followed by 63 '0's


    public:
	class Pos {
	private:
	    /// holds the current position in the automaton
	    int position_;
	    /// holds the current position in the pattern
	    int pattern_pos_;
    
	public:
	    Pos(int position = 0, int pattern_pos = 0) {
		position_ = position;
		pattern_pos_ = pattern_pos;
	    }

	    inline int position() const {return position_;}
	    inline int pattern_pos() const {return pattern_pos_;}
	    inline void set(int position,int pattern_pos) {position_=position; pattern_pos_ = pattern_pos;};
    
	};

	LevDEA(const Alphabet& alphabet,int init_k);


	~LevDEA();
    
	Pos LevDEA::walk(const Pos& p,int c) const;


	inline bool isFinal(const Pos& p) const {
	    //      |  triangle has reached right bound |    |       fin_table gives dist >-1                           |
	    return ((patLength - p.pattern_pos() < 2*k+1) && (fin_table(2*k-(patLength-p.pattern_pos()),p.position())!=-1));    
	}

	inline int getDist(const Pos& p) const {
	    if(patLength - p.pattern_pos() >= 2*k+1) return -1; 
	    return fin_table(2*k-(patLength-p.pattern_pos()),p.position());    
	}

	

	uchar* getPattern() {
	    return pattern_;
	}

	void initDistance();
	void loadPattern(const uchar* p);
    
	// used for debug only
	void printTable() const;
	void printCharvec() const;
	void printBits(const bits64&) const;
    };




} // eon

#endif
