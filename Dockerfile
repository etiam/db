FROM ubuntu:16.04

MAINTAINER etiam "https://github.com/etiam"

RUN apt-get update
RUN apt-get install -y build-essential wget git autoconf-archive software-properties-common sudo vim pkg-config
RUN apt-get clean

ENV LANG="en_US.UTF-8" \
    CXX="clang++" \
    CC="clang"

ADD . /home/travis
WORKDIR /home/travis
