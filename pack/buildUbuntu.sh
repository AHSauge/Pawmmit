#!/bin/bash
# Install prerequisites and build a release version of Pawmmit on Ubuntu.
set -e

sudo apt update
sudo apt install -y build-essential libgl1-mesa-dev meson ninja-build pkg-config \
                    python3 git cmark \
                    qt6-base-dev qt6-tools-dev qt6-tools-dev-tools libqt6core5compat6-dev \
                    libgit2-dev libssh2-1-dev libhunspell-dev libcmark-dev liblua5.4-dev

cd "$(dirname "$0")/.."
git pull

meson setup build --buildtype=release
meson compile -C build
