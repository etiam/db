#!/bin/bash

locale-gen en_US en_US.UTF-8
dpkg-reconfigure locales

# add for llvm-5.0
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main"

sudo apt-get update -qq

# for db
sudo apt-get install -qq qt5-default qtbase5-dev libqt5webkit5-dev libclang-5.0-dev libboost-filesystem-dev libboost-python-dev libboost-program-options-dev python-dev llvm-5.0 clang++-5.0 

# for libut
sudo apt-get install -qq libboost-iostreams-dev libboost-test-dev binutils-dev libilmbase-dev libglm-dev

sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-5.0 50 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-5.0
sudo update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-5.0 50
