#! /bin/bash

set -e

cmake -DBUILD_TESTING=ON .
make
ctest --output-on-failure
