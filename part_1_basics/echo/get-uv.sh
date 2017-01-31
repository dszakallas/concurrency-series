#!/usr/bin/env bash

set -e

ver=1.7.0

curl -O http://dist.libuv.org/dist/v$ver/libuv-v$ver.tar.gz
curl -O http://dist.libuv.org/dist/v$ver/libuv-v$ver.tar.gz.sign
gpg --keyserver pool.sks-keyservers.net --recv-keys FDF519364458319FA8233DC9410E5553AE9BC059
gpg --verify libuv-v$ver.tar.gz.sign
tar -xf libuv-v$ver.tar.gz && cd libuv-v$ver
sh autogen.sh
./configure
make install
