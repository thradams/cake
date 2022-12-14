
# Description

Cake is a C23 compiler front end written from scratch in C.

The compiler can be used to translate new versions of C (like C23)
to C99.

It also can be used to try out new features for the C language. 

Extensions like lambdas, defer and more were implemented.

# Web Playground

This is the best way to try.

http://thradams.com/cake/playground.html

# Use cases

If you have a project that is distributed with code, you don't need to limit 
the project development at the lower supported language version.  For instance,
you can use attributes like *nodiscard* during the development or *defer*, both features 
improving the code security. Then adding a extra step in your build you can distribute
a readable C99 source code that compiles everywhere.


# Features

* C23 preprocessor
* Syntax analysis
* Semantic analysis
* C backend
* Builds AST 

# Build

## MSVC build instructions
Open the Developer Command Prompt of visual studio. Go to the *src* directory and type

```
cl build.c && build
```

This will build *cake.exe*, then run cake on its own source code.


## GCC (linux) build instructions
Got to the *src* directory and type:

```
gcc build.c -o build && ./build
```

To run unit tests windows/linux add *-DTEST* for instance:

```
gcc -DTEST build.c -o build && ./build
```

## Emscripten build instructions (web)
Emscripten https://emscripten.org/  is required. 

First do the normal build. 

The normal build also generates a file *lib.c* that is the amalgameted  version of the "core lib".

Then at _./src_ dir type:

```
call emcc -DMOCKFILES "lib.c" -o "Web\cake.js" -s WASM=0 -s EXPORTED_FUNCTIONS="['_CompileText']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"
```

This will generate the *\src\Web\cake.js*


# Running cake at command line

Make sure cake is on your system path.

Samples

```
cake source.c
```

this will ouput *./out/source.c*

See [Manual](manual.html)



# Road map

* Complete semantic analisys + static analysis
* Backend ?
  
 
# References

 A copy of each C standard draft in included in *docs* folder. The parser is based on C23.

 * https://en.wikipedia.org/wiki/ANSI_C#C89
 * https://en.cppreference.com/w/c
 * https://www.ibm.com/docs/en/xl-c-aix/13.1.0?topic=extensions-c99-features

# Influenced by

* Typescript
* Small C compilers

# Participating

You can contribute by trying out cake, reporting bugs, and giving feedback.

Have a sugestion for C?

# Status?
Even if not necessary for most features, I want Cake to make a full semantic analysis.

It is necessary more work it is not complete yet.


# How cake is tested?

I am using Visual Studio 2022 IDE to write/debug cake source. Cake is parsing itself using
the includes of MSVC and it generates the *out* dir after build. I use WSL for testing
and compiling the code for linux.

Cake source code is not using any extension so the output is the same of input. 
This compilation is usefull for tracking errors.

Cake uses unit tests that are called automatically.

# Differences  from CFront

Cfront was the original compiler for C++ which converted C++ to C. 
CFront generated code was used only for direct compilation because it
had all macros expanded making it useless in other plataforms.

Cake have two modes. One is for direct compilation (like cfront) and the other
preserves macros includes etc.. making it suitable for distribution.

The other difference is that C++ is a second branch of evolution making C++ more
compatible with C89 than C99.

The idea of Cake is to keep the main line of evolution of C and be always 100% 
compatible. 

The added extensions aims to keep the spirit of the language and implement proposed 
features in a way they can be experimented even before standarization.









 
 
  
 



