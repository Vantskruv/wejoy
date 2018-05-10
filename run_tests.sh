#!/usr/bin/env bash
cd cmake-build-debug/tests
echo "Running Tests on the Modified Architecture Code"
sudo ./tests -o ../../modified_architecture.log
cd ../../
echo "Running Tests on the original code"
cd original_code/tests/cmake-build-debug
sudo ./tests -o ../../../original_code.log