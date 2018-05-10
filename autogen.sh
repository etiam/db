#!/bin/bash

git submodule update --init
autoreconf --force --verbose --install --warnings=all

# from the docs it looks like AX_SUBDIRS_CONFIGURE should work
# like AC_CONFIG_SUBDIRS and make the following unneccesary but nope.
pushd src/external/libut
./autogen.sh
popd
