#!/bin/bash
echo "building cmake in release"
cmake -DCMAKE_BUILD_TYPE=Release .
./dev.sh