#!/bin/bash

PRESENT_DIR=$(basename `pwd`)

# make sure we're inside the sh folder
if [[ $PRESENT_DIR != "sh" ]]; then
    echo "Please run this script from the sh/ folder."
    exit 1
fi

pushd "$(dirname "$0")"
trap popd EXIT

read -p 'This will install Nim packages and possibly overwrite yours with updated versions. Continue? [y/n]: ' comp

set -e # quits the script as soon as an error happens

if [[ $comp == "y" ]]; then
    echo -e "\tChecking for Nim"
    nim --version

    echo -e "\tChecking for Nimble"
    nimble --version

    echo -e "\tInstalling Nimble packages"
    nimble install cligen -y

    cd ../src

    echo -e "\tStart compiling!"

    nim c -d:release main.nim
    mv main ../sh/bgmplayer

    echo -e "\tBuild finished! (Check sh/ folder)"
else
    exit 1
fi

echo -e "\n\tCompile Success!\n"