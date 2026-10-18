#!/bin/bash
doxy="$0"
basename="$(dirname $doxy)"
myFolder="/generated/apidoc-python"

pushd $basename 
cwd=`pwd`

DOXYGEN_OUTPUT="$cwd$myFolder"

if [[ -d  "$DOXYGEN_OUTPUT" ]]; 
then
	\rm -rf $DOXYGEN_OUTPUT
fi

mkdir -p $DOXYGEN_OUTPUT

cd ../../python-wrapper/src

(
    cat Doxyfile && echo OUTPUT_DIRECTORY = $DOXYGEN_OUTPUT
) | "/usr/bin/doxygen" - 

popd