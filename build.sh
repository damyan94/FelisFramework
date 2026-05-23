#!/bin/bash

mkdir -p "Build"
cd "Build"
cmake ..
cmake --build . -j4 -k0
cd ..
