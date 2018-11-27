#!/bin/bash
set -e

apt-get install -y clang \
extra-cmake-modules cmake \
lsb-release dpkg-dev \
libglvnd-dev liblua5.1-0-dev libglew-dev libjpeg-turbo8-dev libpng++-dev libarchive-dev \
libsdl1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
