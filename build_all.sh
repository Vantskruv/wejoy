#!/usr/bin/env bash
rm -R cmake-build-debug
mkdir cmake-build-debug
cd cmake-build-debug
echo "Building code with architectural changes"
cmake ../
make
cd ..
cd original_code
echo "Building original code"
./make.sh
cd tests
rm -R cmake-build-debug
mkdir cmake-build-debug
cd cmake-build-debug
cmake ../
make
cd ..