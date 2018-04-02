#!/bin/bash

sudo apt-get update -qq
sudo apt-get install -y -qq qt5-default qtbase5-dev libqt5webkit5-dev libclang-5.0-dev libboost-filesystem1.58-dev libboost-python1.58-dev libboost-program-options1.58-dev python-dev llvm-5.0 clang-5.0

git submodule update --init
