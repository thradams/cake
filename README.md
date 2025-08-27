
The C Programming language 1978

> _"C is a general-purpose programming language which features economy of expression, modern control flow and data structures, and a rich set of operators. C is not a "very high level" language, nor a "big" one, and is not specialized to any particular area of application. But its absence of restrictions and its generality make it more convenient and effective for many tasks than supposedly more powerful languages."_

> _"In our experience, C has proven to be a pleasant, expressive, and versatile language for a wide variety of programs. It is easy to learn, and it wears well as one's experience with it grows"_

The C Programming language Second Edition 1988

> _"As we said in the preface to the first edition, C "wears well as one's experience with it grows." With a decade more experience, we still feel that way."_




# 🍰 Cake

Cake is a compiler front-end written from scratch in C, adhering to the C23 language 
specification and beyond.

It serves as a platform for experimenting with new features, 
including C2Y language proposals, safety enhancements, and extensions such as 
literal functions and defer statements.

The current backend generates C89-compatible code, which can be pipelined with existing 
compilers to produce executables. 

Cake aims to enhance C's safety by providing high-quality warning messages and advanced 
flow analysis, including [object lifetime](ownership.md) checks.

# Web Playground

This is the best way to try.

http://thradams.com/cake/playground.html

# Use cases

Note: Cake is still in development and has not yet reached 
a stable version.

Cake can be used as a static analyzer alongside other compilers. 
It generates SARIF files, which are recognized by popular IDEs 
such as Visual Studio and Visual Studio Code, providing a 
seamless integration.

It can also function as a preprocessor, converting C23 code to C89. 
This allows developers to use modern or experimental features while targeting 
compilers that do not yet support the latest language standards.

The objective of the C89 generation is to produce low-level C code, 
simplifying the process of writing C backends **dedicated to code generation**.

Previous versions of Cake included a conversion mode to 
translate code while preserving the preprocessor parts. 
Although useful, this process could not guarantee 100% conversion,
functioning more as a tool than a compiler. With the new versions 
of Cake moving toward becoming a more traditional compiler,
the previous mode has been discontinued at version 0.9.36.
This version is online here 
http://thradams.com/cake3/playground.html.

# Features

* C23 preprocessor
* C23 syntax analysis
* C23 semantic analysis
* Static [object lifetime](ownership.md) checks (Extension)
* Sarif output
* Backend generating C89 compatible code
* AST 
* More than 260 diagnostics


# Build

GitHub 
https://github.com/thradams/cake

## MSVC build instructions
Open the Developer Command Prompt of visual studio. Go to the *src* directory and type

```
cl build.c && build
```

This will build *cake.exe*, then run cake on its own source code.


## GCC on Linux build instructions
Got to the *src* directory and type:

```
gcc build.c -o build && ./build
```

## Clang on Linux/Windows/MacOS build instructions
Got to the *src* directory and type:

```
clang build.c -o build && ./build
```


To run unit tests windows/linux add *-DTEST* for instance:

```
gcc -DTEST build.c -o build && ./build
```

## Emscripten build instructions (web)

Emscripten https://emscripten.org/  is required. 

First do the normal build. 

The normal build also generates a file *lib.c* that is the amalgamated  version of the "core lib".

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

this will output *./out/source.c*

See [Manual](manual.md)



# Road map

* Making it usable as C89 backend and fixes
* Flow v2 algorithm was delayed

# References

https://www.c-language.org/

A very nice introduction was written by Al Williams

C23 Programming For Everyone

https://hackaday.com/2022/09/13/c23-programming-for-everyone/


# Participating

You can contribute by trying out cake, reporting bugs, and giving feedback.

Have a suggestion for C?
  
DISCORD SERVER

[https://discord.gg/YRekr2N65S](https://discord.gg/YRekr2N65S)


# How cake is developed?

I am using Visual Studio 2022 IDE to write/debug cake source. Cake is parsing itself using
the includes of MSVC and it generates the *out* dir after build. 

I use Visual Studio code with WSL for testing and compiling the code for Linux.

Cake source code is not using any extension so the output is the same of input. 
This compilation is useful for tracking errors together with the unit tests.

# Cake is not C++
CFront was the original C++ compiler, designed to translate C++ code into C.

Although CFront was initially compatible with C89, it eventually diverged from C’s evolution.

In contrast, Cake stays true to the core development of C, ensuring full compatibility.

Its extensions aim to preserve the essence of C while allowing developers to experiment and contribute to C’s evolution using an open-source compiler written in C.


 
 
  
 



