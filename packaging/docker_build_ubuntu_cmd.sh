#!/usr/bin/env bash

OS_VER=20.04
docker build -f ./Dockerfile-yastack-ubuntu \
--build-arg OS_VER=${OS_VER} \
-t yastack/ubuntu:${OS_VER} .
