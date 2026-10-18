#!/bin/bash
doxy="$0"
basename="$(dirname $doxy)"
myFolder="/generated/apidoc-cpp"

pushd $basename 
cwd=`pwd`

DOXYGEN_OUTPUT="$cwd$myFolder"

if [[ -d  "$DOXYGEN_OUTPUT" ]]; 
then
	\rm -rf $DOXYGEN_OUTPUT
fi

mkdir -p $DOXYGEN_OUTPUT

cd /../../src
(
    cat Doxyfile && echo OUTPUT_DIRECTORY = $DOXYGEN_OUTPUT
) | "/usr/bin/doxygen" - 

popd