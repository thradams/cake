# Cake

Cake is a C23 compiler frond end written from scratch in C.

The compiler can be used to translate new versions of C (like C23)
to C99.

It also can be used to try out new features for the C language. 

Extensions like lambdas,  defer and more where implemented.

# Web Playground

This is the best way to try.

http://thradams.com/web3/playground.html


# Features

* C23 preprocessor
* syntax analisys
* semantic analisys
* c backend
* Builds AST 

# Build

## Windows
Open the Developer Command Prompt of visual studio. Go to the src directoy and type

```
cl build.c && build
```

This will build `cake.exe`.



## Linux
Got to the `src` directory and type:

```
gcc build.c -o build && ./build
```

To run unit tests windows/linux add `-DTEST` for instance:

```
gcc -DTEST build.c -o build && ./build
```

## Build web
emscriptem https://emscripten.org/  is required. 

First do the normal build. 

The normal build also generates a file lib.c that is the amalgameted  version of the "core lib".

Then at `./src` dir type:

```
call emcc -DMOCKFILES "lib.c" -o "Web\cake.js" -s WASM=0 -s EXPORTED_FUNCTIONS="['_CompileText']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"
```

This will generate the cake.js



# Command line

Make sure cake is on your system path.

Samples

```
cake source.c
```
this will ouput `./out/source.c`

See [Manual](manual.md)



# Road map

 * Complete semantic analisys
 * Static analysis
 * Backend, maybe qbe https://c9x.me/compile/?
 
 
 # References
 A copy of each C standard draft in included in `docs` folder. The parser is based on C23.
 
 # Influenced by
 * Typescript
 * Small C compilers
 
# How cake is tested?

I am using Visual Studio 2022 IDE to write/debug cake source. Cake is parsing itself using
the includes of MSVC and it generates the `out` dir after build.

Cake source code is not using any extension so the output is the same of input. 
This compilation is usefull for tracking errors.







 
 
  
 



