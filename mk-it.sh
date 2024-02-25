#!/bin/sh
cwd=$PWD
cd src
#gcc-13-env
gcc -g build.c -o build && ./build
#emsdk-env emcc -Wno-multichar -DMOCKFILES "lib.c" -o "web\cake.js" -s WASM=0 -s EXPORTED_FUNCTIONS=_CompileText -s EXPORTED_RUNTIME_METHODS=ccall,cwrap
cd $cwd
#myvalgrind src/cake -Iinclude /home/mingo/dev/dadbiz++/third-party/dad/sqlite3-orig/sqlite3.c
#myvalgrind ./cake -E /home/mingo/dev/dadbiz++/third-party/dad/sqlite3-orig/sqlite3.c
#./cake -analyze -D__x86_64__  -I/usr/lib/gcc/x86_64-linux-gnu/11/include/  -I/usr/local/include/  -I/usr/include/x86_64-linux-gnu/  -I/usr/include/  console.h  tokenizer.h  parser.h  error.h  fs.h  hash.h  object.h  hashmap.h  osstream.h  options.h  token.h  type.h  pre_expressions.h  expressions.h  visit.h  format_visit.h  token.c  hash.c  hashmap.c  console.c  tokenizer.c  osstream.c  fs.c  options.c  expressions.c  pre_expressions.c  type.c  object.c  parser.c  visit.c  flow_visit.c  error.c  format_visit.c  tests.c
#./cake -analyze  -I/usr/lib/gcc/x86_64-linux-gnu/11/include/  -I/usr/local/include/  -I/usr/include/x86_64-linux-gnu/  -I/usr/include/  console.c
#myvalgrind --vgdb-error=0 src/cake -Iinclude /home/mingo/dev/dadbiz++/third-party/dad/sqlite3-orig/sqlite3.c
#myvalgrind src/cake -Iinclude dad/test-va.c
#src/cake -dump-tokens dad/test-inf-nan.c
#src/cake -dump-tokens dad/test-wchar.c
#src/cake  dad/test-type-pointer.c
#src/cake  dad/test-align.c
#src/cake  dad/test-double-free.c
#src/cake  dad/test-generic.c
#src/cake  dad/test-trigraph.c