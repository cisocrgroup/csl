########## DIRECTORIES #######################
BIN = ./bin
OBJS = ./objs

######## COMPILER AND FLAGS ###################
#GCC = g++ -O3 -Wall -fpermissive # use this for optimal speed results
#GCC = g++ -O3 -Wall # use this for optimal speed results
#GCC = g++ -Wall --no-inline
GCC = g++ -ggdb -Wall # use this if you want to use the gdb debugger



GLOBAL_H = ./Global.h
ALPHABET_H = ./Alphabet/Alphabet.h $(GLOBAL_H)
$(OBJS)/Alphabet.o: $(ALPHABET_H) ./Alphabet/Alphabet.cxx
	$(GCC) -c ./Alphabet/Alphabet.cxx -o $(OBJS)/Alphabet.o


########## DAWG AND RELATED ###################

TRANSTABLE_FILES = ./TransTable/TransTable.h ./TransTable/TransTable.cxx ./TransTable/Cell.h ./TransTable/TempState.h $(GLOBAL_H) $(ALPHABET_H)

HASH_FILES = ./Hash/Hash.h

TRIE_FILES = ./Trie/Trie.h ./Trie/Trie.cxx $(TRANSTABLE_FILES)

MINDIC_FILES = ./MinDic/MinDic.h ./MinDic/MinDic.cxx ./MinDic/StateHash.h $(TRANSTABLE_FILES) $(HASH_FILES)
$(OBJS)/MinDic.o: $(MINDIC_H) ./MinDic/MinDic.cxx
	$(GCC) -c ./MinDic/MinDic.cxx -o $(OBJS)/MinDic.o


########## LEVFILTER AND RELATED #############
LEVFILTER_H = ./LevFilter/LevFilter.h

LEVDEA_H = ./LevDEA/LevDEA.h $(GLOBAL_H) $(ALPHABET_H)
$(OBJS)/LevDEA.o: $(LEVDEA_H) ./LevDEA/LevDEA.cxx
	$(GCC) -c ./LevDEA/LevDEA.cxx -o $(OBJS)/LevDEA.o

MSMATCH_H = ./MSMatch/MSMatch.h $(LEVFILTER_H) $(GLOBAL_H) $(LEVDEA_H) $(TRIE_H)
$(OBJS)/MSMatch.o: $(MSMATCH_H) ./MSMatch/MSMatch.cxx
	$(GCC) -c ./MSMatch/MSMatch.cxx -o $(OBJS)/MSMatch.o

LEVNDEA_H = ./LevNDEA/LevNDEA.h $(GLOBAL_H) $(ALPHABET_H)
$(OBJS)/LevNDEA.o: $(LEVNDEA_H) ./LevNDEA/LevNDEA.cxx
	$(GCC) -c ./LevNDEA/LevNDEA.cxx -o $(OBJS)/LevNDEA.o

BESTMATCH_H = ./BestMatch/BestMatch.h $(LEVFILTER_H) $(GLOBAL_H) $(LEVNDEA_H) $(TRIE_H) $(RESULTSET_H) 
$(OBJS)/BestMatch.o: $(BESTMATCH_H) ./BestMatch/BestMatch.cxx
	$(GCC) -c ./BestMatch/BestMatch.cxx -o $(OBJS)/BestMatch.o

RESULTSET_H = ./ResultSet/ResultSet.h $(LEVFILTER_H) $(GLOBAL_H) 
$(OBJS)/ResultSet.o: $(RESULTSET_H) ./ResultSet/ResultSet.cxx
	$(GCC) -c ./ResultSet/ResultSet.cxx -o $(OBJS)/ResultSet.o


########### PermuteMatch OLD ####################
PERMUTEMATCH_OLD_H = ./PermuteMatch/PermuteMatch.h ./PermuteMatch/List.h $(GLOBAL_H) $(MSMATCH_H) $(TRIE_H) 
$(OBJS)/PermuteMatch_old.o: $(PERMUTEMATCH_OLDH) ./PermuteMatch/PermuteMatch.cxx
	$(GCC) -c ./PermuteMatch/PermuteMatch.cxx -o $(OBJS)/PermuteMatch_old.o


########### StructMatch #####################
COMPLETEMATCH_H = ./StructMatch/CompleteMatch/CompleteMatch.h ./StructMatch/CompleteMatch/List.h $(GLOBAL_H) $(MSMATCH_H) $(TRIE_H) 
$(OBJS)/CompleteMatch.o: $(PERMUTEMATCH_H) ./StructMatch/CompleteMatch/CompleteMatch.cxx
	$(GCC) -c ./StructMatch/CompleteMatch/CompleteMatch.cxx -o $(OBJS)/CompleteMatch.o

PERMUTEMATCH_H = ./StructMatch/PermuteMatch/PermuteMatch.h ./StructMatch/PermuteMatch/List.h $(GLOBAL_H) $(MSMATCH_H) $(TRIE_H) 
$(OBJS)/PermuteMatch.o: $(PERMUTEMATCH_H) ./StructMatch/PermuteMatch/PermuteMatch.cxx
	$(GCC) -c ./StructMatch/PermuteMatch/PermuteMatch.cxx -o $(OBJS)/PermuteMatch.o

PERMDIC_H = ./StructMatch/PermDic/PermDic.h ./StructMatch/PermDic/Entry.h
$(OBJS)/PermDic.o: $(PERMDIC_H) ./StructMatch/PermDic/PermDic.cxx
	$(GCC) -c ./StructMatch/PermDic/PermDic.cxx -o $(OBJS)/PermDic.o


########### BINARIES ########################

$(BIN)/stats: ./TransTable/stats.cxx $(TRANSTABLE_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/stats ./TransTable/stats.cxx $(OBJS)/Alphabet.o

$(BIN)/trieToDot: ./Trie/trieToDot.cxx $(OBJS)/Trie.o $(TRANSTABLE_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/trieToDot ./Trie/trieToDot.cxx $(OBJS)/Trie.o $(OBJS)/Alphabet.o

$(BIN)/compileTrie: ./Trie/compileTrie.cxx $(TRANSTABLE_FILES) $(TRIE_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/compileTrie ./Trie/compileTrie.cxx $(OBJS)/Alphabet.o

$(BIN)/compileMD: ./MinDic/compileMD.cxx $(TRANSTABLE_FILES) $(MINDIC_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/compileMD ./MinDic/compileMD.cxx $(OBJS)/Alphabet.o

$(BIN)/compilePD: ./StructMatch/PermDic/compilePD.cxx $(OBJS)/PermDic.o $(TRANSTABLE_FILES) $(OBJS)/MinDic.o $(OBJS)/Alphabet.o $(TRANSTABLE_FILES)
	$(GCC) -o $(BIN)/compilePD StructMatch/PermDic/compilePD.cxx $(OBJS)/MinDic.o $(OBJS)/PermDic.o $(OBJS)/Alphabet.o

$(BIN)/extractTrie: ./Trie/extractTrie.cxx $(TRANSTABLE_FILES) $(TRIE_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/extractTrie ./Trie/extractTrie.cxx $(OBJS)/Alphabet.o

$(BIN)/extractMD: ./MinDic/extractMD.cxx $(TRANSTABLE_FILES) $(MINDIC_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/extractMD ./MinDic/extractMD.cxx $(OBJS)/Alphabet.o

$(BIN)/bestMatch: ./BestMatch/bestMatch.cxx $(OBJS)/BestMatch.o $(OBJS)/ResultSet.o $(OBJS)/LevNDEA.o $(TRANSTABLE_FILES) $(MINDIC_FILES) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/bestMatch ./BestMatch/bestMatch.cxx $(OBJS)/BestMatch.o $(OBJS)/ResultSet.o $(OBJS)/LevNDEA.o $(OBJS)/Alphabet.o


$(BIN)/msFilter: ./MSMatch/msFilter.cxx $(OBJS)/MSMatch.o $(OBJS)/ResultSet.o $(OBJS)/LevDEA.o $(TRANSTABLE_FILES) $(OBJS)/Trie.o $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/msFilter ./MSMatch/msFilter.cxx $(OBJS)/MSMatch.o $(OBJS)/ResultSet.o $(OBJS)/LevDEA.o $(OBJS)/Trie.o  $(OBJS)/Alphabet.o


getCandscore_files = ./getCandscore/getCandscore.cxx $(OBJS)/MSMatch.o $(OBJS)/ResultSet.o $(OBJS)/LevDEA.o $(TRANSTABLE_FILES) $(OBJS)/Trie.o $(OBJS)/Alphabet.o
$(BIN)/getCandscore: $(getCandscore_files)
	$(GCC) $(getCandscore_files) -o $(BIN)/getCandscore

permuteMatch_old_files = ./PermuteMatch/permuteMatch.cxx $(OBJS)/PermuteMatch_old.o $(OBJS)/Alphabet.o $(OBJS)/MSMatch.o $(OBJS)/LevDEA.o
$(BIN)/permuteMatch_old: $(permuteMatch_old_files)
	$(GCC) $(permuteMatch_old_files) -o $(BIN)/permuteMatch_old

completeMatch_files = ./StructMatch/CompleteMatch/completeMatch.cxx $(OBJS)/CompleteMatch.o $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o
$(BIN)/completeMatch: $(completeMatch_files) $(TRANSTABLE_FILES)
	$(GCC) $(completeMatch_files) -o $(BIN)/completeMatch

permuteMatch_files = ./StructMatch/PermuteMatch/permuteMatch.cxx $(OBJS)/PermuteMatch.o $(OBJS)/Alphabet.o $(TRANSTABLE_FILES) $(OBJS)/MSMatch.o $(OBJS)/LevDEA.o $(OBJS)/ResultSet.o
$(BIN)/permuteMatch: $(permuteMatch_files) $(TRANSTABLE_FILES)
	$(GCC) $(permuteMatch_files) -o $(BIN)/permuteMatch


########### DIV ###########################
clean:
	rm -v $(OBJS)/* $(BIN)/*
########## DIRECTORIES #######################
BIN = ./bin
OBJS = ./objs
