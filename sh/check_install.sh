#!/bin/bash

set -e # quits the script as soon as an error happens

# todo check to make sure nim is at least version 1.6
echo -e "\tChecking for Nim"
nim --version

echo -e "\tChecking for Nimble"
nimble --version
