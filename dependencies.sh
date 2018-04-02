#!/bin/bash

# add for llvm-5.0
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -

sudo apt-get update -qq
sudo apt-get install -qq qt5-default qtbase5-dev libqt5webkit5-dev libclang-5.0-dev libboost-filesystem-dev libboost-python-dev libboost-program-options-dev python-dev llvm-5.0 clang-5.0

git submodule update --init
