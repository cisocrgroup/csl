#
# This shell script builds a number of executables
# It is designed as an alternative to the cmake build generator,
# in case it is not installed or nt running properly.
# 
# Another advantage of executables built that way that they are stand-alone,
# they do not rely on libcsl.so .


#!/bin/sh


COMPILER='g++ -O3 -I.'
LIBSOURCES='csl/Alphabet/Alphabet.cxx csl/LevDEA/LevDEA.cxx csl/DictSearch/DictSearch.cxx csl/Pattern/Pattern.cxx  csl/Pattern/ComputeInstruction.cxx csl/Pattern/Instruction.cxx csl/Pattern/Interpretation.cxx csl/Pattern/PatternWeights.cxx'

mkdir -vp bin

while test "$1" != "" ; do
    case $1 in
	compileMD)
	    echo "Build compileMD"
	    $COMPILER -o bin/compileMD tools/compileMD/compileMD.cxx $LIBSOURCES
	    ;;
	extractMD)
	    echo "Build extractMD"
	    $COMPILER -o bin/extractMD tools/extractMD/extractMD.cxx $LIBSOURCES
	    ;;
	compileFBDic)
	    echo "Build compileFBDic"
	    $COMPILER -o bin/compileFBDic tools/compileFBDic/compileFBDic.cxx $LIBSOURCES
	    ;;
	extractFBDic)
	echo "Build extractFBDic"
	    $COMPILER -o bin/extractFBDic tools/extractFBDic/extractFBDic.cxx $LIBSOURCES
	    ;;
	compileMD2)
	    echo "Build compileMD2"
	    $COMPILER -o bin/compileMD2 tools/compileMD2/compileMD2.cxx $LIBSOURCES
	    ;;
	extractMD2)
	    echo "Build extractMD2"
	    $COMPILER -o bin/extractMD2 tools/extractMD2/extractMD2.cxx $LIBSOURCES
	    ;;
	msFilter)
	    echo "Build msFilter"
	    $COMPILER -o bin/msFilter tools/msFilter/msFilter.cxx $LIBSOURCES
	    ;;
	vaamFilter)
	    echo "Build vaamFilter"
	    $COMPILER -o bin/vaamFilter tools/vaamFilter/vaamFilter.cxx $LIBSOURCES
	    ;;
	valFilter)
	    echo "Build valFilter"
	    $COMPILER -o bin/valFilter tools/valFilter/valFilter.cxx $LIBSOURCES
	    ;;
    esac
    shift
done
