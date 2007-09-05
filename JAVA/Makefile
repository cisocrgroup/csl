
csl/candidateExtractor/CandidateExtractor.class: csl/candidateExtractor/CandidateExtractor.java
	javac csl/candidateExtractor/CandidateExtractor.java

csl/candidateExtractor/CandidateExtractor.h: csl/candidateExtractor/CandidateExtractor.class
	javah csl.candidateExtractor.CandidateExtractor
	cp -v csl_candidateExtractor_CandidateExtractor.h csl/candidateExtractor/CandidateExtractor.h


./libCandidateExtractor.so: csl/candidateExtractor/CandidateExtractor.cxx csl/candidateExtractor/CandidateExtractor.h ../objs/LevDEA.o ../objs/MinDic.o  ../objs/Alphabet.o
	g++ -o libCandidateExtractor.so -shared -fpic csl/candidateExtractor/CandidateExtractor.cxx ../objs/LevDEA.o ../objs/MinDic.o  ../objs/Alphabet.o

./csl.jar: 
	jar cf csl.jar csl/
