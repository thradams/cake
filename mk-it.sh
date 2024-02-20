#!/bin/sh
cwd=$PWD
cd src
gcc -g build.c -o build && ./build
emsdk-env emcc -Wno-multichar -DMOCKFILES "lib.c" -o "web\cake.js" -s WASM=0 -s EXPORTED_FUNCTIONS=_CompileText -s EXPORTED_RUNTIME_METHODS=ccall,cwrap
cd $cwd
