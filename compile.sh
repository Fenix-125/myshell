#!/bin/bash -x

set -o errexit
set -o nounset
set -o pipefail

mkdir -p bin build
(
  cd build
  cmake -G"Unix Makefiles" ..
  make
  cp ./myshell ../bin
  mv ./myshell ../
  mv ./programs/mycat/mycat ../bin
  mv ./programs/mycp/mycp ../bin
)
#rm -r build
