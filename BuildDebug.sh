#!/bin/bash
echo "building cmake in debug"
cmake -DCMAKE_BUILD_TYPE=Debug .
./dev.sh