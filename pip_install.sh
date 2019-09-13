#/bin/bash
set -e
export BIN_DIR="$PWD/bin"
#echo "BIN_DIR = $BIN_DIR" > cmake_build.log
rm -rf "$BIN_DIR"
cmake -H. -B"$BIN_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBRARY=1
cd "$BIN_DIR"
make
cd ..
