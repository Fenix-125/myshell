#!/bin/bash -x

set -o errexit
set -o nounset
set -o pipefail

mkdir -p bin build
(
  cd build
  cmake -G"Unix Makefiles" ..
  make
  mv ./myshell ../
  mv ./mycat/mycat ../bin
)
#rm -r build
