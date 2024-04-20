#!/bin/bash

SCRIPTDIR=$(realpath $(dirname ${BASH_SOURCE[0]}))
ROOT=$(realpath $SCRIPTDIR)

cmake -S $ROOT -B $ROOT/build/tests -DVoiceSDK_TESTS=ON
cmake --build $ROOT/build/tests --config Release
cd $ROOT/build/tests
ctest --verbose --output-on-failure -C Release