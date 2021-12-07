#!/bin/bash

read -p 'Would you like to compile the source code? [y/n]: ' comp

set -e # quits the script as soon as an error happens

COMPILE_SUCCESS=false
if [[ $comp == "y" ]]; then
    HAS_CMAKE=false
    
    echo "Checking for CMake"
    cmake --version

    cd ..
    mkdir build || echo "Does the build folder already exist?"
    cd build

    echo "Start compiling!"
    cmake ..
    make
    cp bgmplayer ../sh
    echo "Build finished!"
fi
echo -e "\n\tCompile Success!\n"
COMPILE_SUCCESS=true