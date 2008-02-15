########## DIRECTORIES #######################
BIN = ./bin
OBJS = ./objs
LIB = ./lib
######## COMPILER AND FLAGS ###################

GCC = g++ -O3 -Wall # use this for optimized speed but with assertions being checked
#GCC = g++ -O3 -DNDEBUG -Wall # use this for optimal speed results
#GCC = g++ -Wall --no-inline
#GCC = g++ -ggdb --no-inline -Wall # use this if you want to use the gdb debugger

AR = ar cru
RANLIB = ranlib


GLOBAL_H = ./Global.h


all: Trie MinDic MinDicString MSMatch Perm

install:
	cp -v ./bin/* ~/bin/



ALPHABET_HEADERS = ./Alphabet/Alphabet.h $(GLOBAL_H)
$(OBJS)/Alphabet.o: $(ALPHABET_HEADERS) ./Alphabet/Alphabet.cxx
	$(GCC) -c ./Alphabet/Alphabet.cxx -o $(OBJS)/Alphabet.o




########## DAWG AND RELATED ###################

Trie: ./bin/compileTrie ./bin/extractTrie
MinDic: ./bin/compileMD ./bin/extractMD ./bin/lookupMD
MinDicString: ./bin/compileMDString ./bin/extractMDS ./bin/lookupMDString
Perm: ./bin/permuteMatch ./bin/partPermuteMatch

TRANSTABLE_HEADERS = ./TransTable/TransTable.h ./TransTable/TransTable.cxx ./TransTable/Cell.h ./TransTable/TempState.h $(GLOBAL_H) $(ALPHABET_HEADERS)

RESULTSET_H = ./ResultSet/ResultSet.h

HASH_FILES = ./Hash/Hash.h

TRIE_HEADERS = ./Trie/Trie.h $(ALPHABET_HEADERS) $(TRANSTABLE_HEADERS)
$(OBJS)/Trie.o: $(TRIE_HEADERS) ./Trie/Trie.cxx
	$(GCC) -c ./Trie/Trie.cxx -o $(OBJS)/Trie.o

MINDIC_HEADERS = ./MinDic/MinDic.h ./MinDic/MinDic.tcc ./MinDic/StateHash.h ./MinDic/StateHash.tcc $(TRANSTABLE_HEADERS) $(HASH_FILES)

MINDICSTRING_HEADERS = ./MinDicString/MinDicString.h $(MINDIC_HEADERS) ./Hash/Hash.h
ERRDIC_HEADERS =  ./ErrDic/ErrDic.h ./ErrDic/ErrDic.tcc ./ErrDic/ErrDicConstructor.h ./ErrDic/PatternApplier.h ./ErrDic/Position.tcc  $(MINDIC_HEADERS) ./Hash/Hash.h

FBDic_HEADERS = ./FBDic/FBDic.h ./FBDic/FBDic.tcc $(MINDIC_HEADERS)


########## LEVFILTER AND RELATED #############
MSMatch: ./bin/msFilter

LEVFILTER_HEADERS = ./LevFilter/LevFilter.h

LEVDEA_HEADERS = ./LevDEA/LevDEA.h ./LevDEA/LevDEA_postables.h ./LevDEA/LevDEA_postables.h $(GLOBAL_H) $(ALPHABET_HEADERS)
$(OBJS)/LevDEA.o: $(LEVDEA_HEADERS) ./LevDEA/LevDEA.cxx ./LevDEA/LevDEA_postables.cxx
	$(GCC) -c ./LevDEA/LevDEA.cxx -o $(OBJS)/LevDEA.o

MSMATCH_HEADERS = ./MSMatch/MSMatch.h ./MSMatch/MSMatch.tcc $(LEVFILTER_HEADERS) $(GLOBAL_H) $(LEVDEA_HEADERS) $(FBDIC_HEADERS)

LEVNDEA_HEADERS = ./LevNDEA/LevNDEA.h ./LevNDEA/SuggestIter.h $(GLOBAL_H) $(ALPHABET_HEADERS)
$(OBJS)/LevNDEA.o: $(LEVNDEA_HEADERS) ./LevNDEA/LevNDEA.cxx
	$(GCC) -c ./LevNDEA/LevNDEA.cxx -o $(OBJS)/LevNDEA.o

BESTMATCH_H = ./BestMatch/BestMatch.h $(LEVFILTER_HEADERS) $(GLOBAL_H) $(LEVNDEA_HEADERS) $(TRIE_FILES) $(RESULTSET_H) 
$(OBJS)/BestMatch.o: $(BESTMATCH_H) ./BestMatch/BestMatch.cxx
	$(GCC) -c ./BestMatch/BestMatch.cxx -o $(OBJS)/BestMatch.o

CANDEXTRACTOR_JNI_HEADERS = ./CandExtractor_jni/CandExtractor.h $(MSMATCH_HEADERS) $(RESULTSET_H)
CANDEXTRACTOR_JNI_OBJS =  ./objs/LevDEA.o ./objs/MinDic.o  ./objs/Alphabet.o
$(LIB)/libJNICandExtractor.so: $(CANDEXTRACTOR_JNI_HEADERS) $(CANDEXTRACTOR_JNI_OBJS)
	$(GCC) -o $(LIB)/libJNICandExtractor.so -shared -fpic ./CandExtractor_jni/CandExtractor.cxx $(CANDEXTRACTOR_JNI_OBJS)


########### PermuteMatch OLD ####################
PERMUTEMATCH_OLD_H = ./PermuteMatch/PermuteMatch.h ./PermuteMatch/List.h $(GLOBAL_H) $(MSMATCH_HEADERS) $(TRIE_FILES) 
$(OBJS)/PermuteMatch_old.o: $(PERMUTEMATCH_OLD_H) ./PermuteMatch/PermuteMatch.cxx
	$(GCC) -c ./PermuteMatch/PermuteMatch.cxx -o $(OBJS)/PermuteMatch_old.o


########### BINARIES ########################

$(BIN)/stats: ./TransTable/stats.cxx $(TRANSTABLE_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/stats ./TransTable/stats.cxx $(OBJS)/Alphabet.o

$(BIN)/trieToDot: ./Trie/trieToDot.cxx $(OBJS)/Trie.o $(TRANSTABLE_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/trieToDot ./Trie/trieToDot.cxx $(OBJS)/Trie.o $(OBJS)/Alphabet.o

$(BIN)/compileTrie: ./Trie/compileTrie.cxx $(OBJS)/Trie.o $(OBJS)/Alphabet.o $(TRANSTABLE_HEADERS) $(TRIE_HEADERS) $(ALPHABET_HEADERS)
	$(GCC) -o $(BIN)/compileTrie ./Trie/compileTrie.cxx $(OBJS)/Trie.o $(OBJS)/Alphabet.o

$(BIN)/compileMD: ./MinDic/compileMD.cxx $(TRANSTABLE_HEADERS)  $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/compileMD ./MinDic/compileMD.cxx $(OBJS)/Alphabet.o

$(BIN)/compileMDString: ./MinDicString/compileMDString.cxx $(TRANSTABLE_HEADERS) $(MINDICSTRING_HEADERS)  $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/compileMDString ./MinDicString/compileMDString.cxx $(OBJS)/Alphabet.o

$(BIN)/compileFBDic: ./FBDic/compileFBDic.cxx $(FBDic_HEADERS)  $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/compileFBDic ./FBDic/compileFBDic.cxx $(OBJS)/Alphabet.o

$(BIN)/lookupMDString: ./MinDicString/lookupMDString.cxx $(TRANSTABLE_HEADERS) $(MINDICSTRING_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/lookupMDString ./MinDicString/lookupMDString.cxx $(OBJS)/Alphabet.o

$(BIN)/compileED: ./ErrDic/compileED.cxx $(ERRDIC_HEADERS)  $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/compileED ./ErrDic/compileED.cxx $(OBJS)/Alphabet.o

$(BIN)/createED: ./ErrDic/createED.cxx $(ERRDIC_HEADERS)  $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o
	$(GCC) -o $(BIN)/createED ./ErrDic/createED.cxx $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o

$(BIN)/lookupED: ./ErrDic/lookupED.cxx $(ERRDIC_HEADERS)  $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/lookupED ./ErrDic/lookupED.cxx $(OBJS)/Alphabet.o

$(BIN)/extractED: ./ErrDic/extractED.cxx $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/extractED ./ErrDic/extractED.cxx $(OBJS)/Alphabet.o

$(BIN)/searchText: ./SearchText/searchText.cxx ./SearchText/SearchText.h  $(TRANSTABLE_HEADERS) $(MINDICSTRING_HEADERS)  $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/searchText ./SearchText/searchText.cxx $(OBJS)/Alphabet.o

$(BIN)/extractTrie: ./Trie/extractTrie.cxx $(TRANSTABLE_HEADERS) $(OBJS)/Trie.o $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/extractTrie ./Trie/extractTrie.cxx $(OBJS)/Trie.o $(OBJS)/Alphabet.o

$(BIN)/trie2dot: ./Trie/trie2dot.cxx $(TRANSTABLE_HEADERS) $(OBJS)/Trie.o $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/trie2dot ./Trie/trie2dot.cxx $(OBJS)/Trie.o $(OBJS)/Alphabet.o

$(BIN)/extractMDS: ./MinDicString/extractMDS.cxx $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) ./MinDicString/MinDicString.h $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/extractMDS ./MinDicString/extractMDS.cxx $(OBJS)/Alphabet.o

$(BIN)/extractMD: ./MinDic/extractMD.cxx $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/extractMD ./MinDic/extractMD.cxx $(OBJS)/Alphabet.o

$(BIN)/lookupMD: ./MinDic/lookupMD.cxx $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/lookupMD ./MinDic/lookupMD.cxx $(OBJS)/Alphabet.o

$(BIN)/bestMatch: ./BestMatch/bestMatch.cxx $(OBJS)/BestMatch.o $(OBJS)/LevNDEA.o $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/bestMatch ./BestMatch/bestMatch.cxx $(OBJS)/BestMatch.o $(OBJS)/LevNDEA.o $(OBJS)/Alphabet.o

$(BIN)/bmFilter: ./BestMatch/bmFilter.cxx $(OBJS)/BestMatch.o $(OBJS)/LevNDEA.o $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) $(RESULTSET_H) $(OBJS)/Alphabet.o  
	$(GCC) -o $(BIN)/bmFilter ./BestMatch/bmFilter.cxx $(OBJS)/BestMatch.o $(OBJS)/LevNDEA.o $(OBJS)/Alphabet.o


$(BIN)/msFilter: ./MSMatch/msFilter.cxx $(MSMATCH_HEADERS) $(RESULTSET_H) $(OBJS)/LevDEA.o $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/msFilter ./MSMatch/msFilter.cxx $(OBJS)/LevDEA.o  $(OBJS)/Alphabet.o

getCandscore_files = ./getCandscore/getCandscore.cxx $(OBJS)/LevDEA.o $(OBJS)/Alphabet.o
$(BIN)/getCandscore: $(getCandscore_files) $(MSMATCH_HEADERS) $(TRANSTABLE_HEADERS) 
	$(GCC) $(getCandscore_files) -o $(BIN)/getCandscore

permuteMatch_old_files = ./PermuteMatch/permuteMatch.cxx $(OBJS)/PermuteMatch_old.o $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o $(MINDIC_HEADERS) $(OBJS)/Trie.o
$(BIN)/permuteMatch_old: $(permuteMatch_old_files) $(MSMATCH_HEADERS) ./PermuteMatch/List.h
	$(GCC) $(permuteMatch_old_files) -o $(BIN)/permuteMatch_old

completeMatch_files = ./StructMatch/CompleteMatch/completeMatch.cxx $(OBJS)/CompleteMatch.o $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o $(OBJS)/Trie.o
$(BIN)/completeMatch: $(completeMatch_files) $(TRANSTABLE_HEADERS)
	$(GCC) $(completeMatch_files) -o $(BIN)/completeMatch


permuteMatch_files = ./StructMatch/PermuteMatch/permuteMatch.cxx $(OBJS)/LevDEA.o $(OBJS)/PermuteMatch.o $(OBJS)/Alphabet.o $(OBJS)/Trie.o
$(BIN)/permuteMatch: $(permuteMatch_files) $(TRANSTABLE_HEADERS)  $(MSMATCH_HEADERS) 
	$(GCC) $(permuteMatch_files) -o $(BIN)/permuteMatch

partPermuteMatch_files = ./StructMatch/PartPermuteMatch/partPermuteMatch.cxx $(OBJS)/LevDEA.o $(OBJS)/PartPermuteMatch.o $(OBJS)/Alphabet.o $(OBJS)/Trie.o
$(BIN)/partPermuteMatch: $(partPermuteMatch_files) $(TRANSTABLE_HEADERS)  $(MSMATCH_HEADERS) 
	$(GCC) $(partPermuteMatch_files) -o $(BIN)/partPermuteMatch


############ TESTS ########################
$(BIN)/testMinDic: ./MinDic/Test/testMinDic.cxx ./MinDic/Test/TestMinDic.h $(TRANSTABLE_HEADERS) $(MINDIC_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/testMinDic ./MinDic/Test/testMinDic.cxx $(OBJS)/Alphabet.o

$(BIN)/testSearchText: ./SearchText/Test/testSearchText.cxx ./SearchText/SearchText.h ./SearchText/Test/TestSearchText.h  $(TRANSTABLE_HEADERS) $(MINDICSTRING_HEADERS) $(OBJS)/Alphabet.o
	$(GCC) -o $(BIN)/testSearchText ./SearchText/Test/testSearchText.cxx $(OBJS)/Alphabet.o


testPermuteMatch_files = ./StructMatch/PermuteMatch/Test/testPermuteMatch.cxx $(OBJS)/LevDEA.o $(OBJS)/PermuteMatch.o $(OBJS)/Alphabet.o $(OBJS)/Trie.o $(MINDIC_HEADERS)
$(BIN)/testPermuteMatch: $(testPermuteMatch_files) $(TRANSTABLE_HEADERS) $(MSMATCH_HEADERS) ./StructMatch/PermuteMatch/Test/TestPermuteMatch.h
	$(GCC) $(testPermuteMatch_files) -o $(BIN)/testPermuteMatch

testPartPermuteMatch_files = ./StructMatch/PartPermuteMatch/Test/testPartPermuteMatch.cxx ./StructMatch/PartPermuteMatch/Test/TestPartPermuteMatch.h $(OBJS)/LevDEA.o $(OBJS)/PartPermuteMatch.o $(OBJS)/Alphabet.o $(OBJS)/Trie.o
$(BIN)/testPartPermuteMatch: $(testPartPermuteMatch_files) $(TRANSTABLE_HEADERS) $(MSMATCH_HEADERS) ./StructMatch/PartPermuteMatch/Test/TestPartPermuteMatch.h
	$(GCC) $(testPartPermuteMatch_files) -o $(BIN)/testPartPermuteMatch



######### libs ##########################
$(LIB)/MSMatch.a: $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o
	$(AR) $(LIB)/MSMatch.a $(OBJS)/Alphabet.o $(OBJS)/LevDEA.o
	$(RANLIB) $(LIB)/MSMatch.a



########### DIV ###########################
clean:
	rm -fv $(OBJS)/* $(BIN)/* $(LIB)/*
