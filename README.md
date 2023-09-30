
The C Programming language 1978

> _"C is a general-purpose programming language which features economy of expression, modern control flow and data structures, and a rich set of operators. C is not a "very high level" language, nor a "big" one, and is not specialised to any particular area of application. But its absence of restrictions and its generality make it more convenient and effective for many tasks than supposedly more powerful languages."_

> _"In our experience, C has proven to be a pleasant, expressive, and versatile language for a wide variety of programs. It is easy to learn, and it wears well as one's experience with it grows"_

The C Programming language Second Edition 1988

> _"As we said in the preface to the first edition, C "wears well as one's experience with it grows." With a decade more experience, we still feel that way."_


Me 2023

> _As my experience with any language grows, more a like C._



# 🍰 Cake

Cake is a compiler front end written from scratch in C, designed from the C23 language specification.
It allows you to translate newer versions of C, such as C23, to C99. Additionally, Cake provides a platform 
for experimenting with new features for the C language, including extensions like lambdas and defer and static  [ownership](ownership.html) checks.


# Web Playground

This is the best way to try.

http://thradams.com/cake/playground.html

# Use cases

If you have a project that is distributed with code, you don't need to limit the project development at the lower supported language version.  For instance, you can use attributes like *nodiscard* during the development or *defer*, both features 
improving the code security. Then adding a extra step in your build you can distribute a readable C99 source code that compiles everywhere.
Cake can also be used as static analyzer, especially the new ownership analysis.

# Features

* C23 preprocessor
* C23 syntax analysis
* C23 semantic analysis (> 58 errors, > 16 warnings)
* static ownership checks (extension)
* sarif output
* C backend
* AST 

# Build

GitHub 
https://github.com/thradams/cake

## MSVC build instructions
Open the Developer Command Prompt of visual studio. Go to the *src* directory and type

```
cl build.c && build
```

This will build *cake.exe*, then run cake on its own source code.


## GCC (Linux) build instructions
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

this will output *./out/source.c*

See [Manual](manual.html)



# Road map

* Ownership static analysis
*
 
# References

How did we get here?
 
 * https://www.bell-labs.com/usr/dmr/www/chist.html
 * https://www.bell-labs.com/usr/dmr/www/cman.pdf
 * https://www.bell-labs.com/usr/dmr/www/ctut.pdf


 A copy of each C standard draft in included in *docs* folder.

 * https://en.wikipedia.org/wiki/ANSI_C#C89
 * https://en.cppreference.com/w/c
 * https://www.ibm.com/docs/en/xl-c-aix/13.1.0?topic=extensions-c99-features
 * https://en.cppreference.com/w/c/compiler_support/23
 * https://en.cppreference.com/w/c/compiler_support/99
	
A very nice introduction was written by Al Williams

C23 Programming For Everyone

https://hackaday.com/2022/09/13/c23-programming-for-everyone/


# Influenced by

* Typescript
* Small C compilers

# Participating

You can contribute by trying out cake, reporting bugs, and giving feedback.

Have a suggestion for C?
  
DISCORD SERVER
https://discord.gg/mtfw8qVw

# Status?

At version 0.5.13 we have 58 types of erros (we have more messages sharing the 
same type) and 14 warnings/info.

# How cake is tested?

I am using Visual Studio 2022 IDE to write/debug cake source. Cake is parsing itself using
the includes of MSVC and it generates the *out* dir after build. 

I use Visual Studio code with WSL for testing and compiling the code for Linux.

Cake source code is not using any extension so the output is the same of input. 
This compilation is useful for tracking errors together with the unit tests.

# Differences  from CFront

CFront was the original compiler for C++ which converted C++ to C. 

CFront generated code was used only for direct compilation because it
had all macros expanded making it useless to reuse the generated code in 
other platforms.

Cake have two modes. One is for direct compilation (like CFront) and the other
preserves macros includes etc.. making it suitable for distribution.

The other difference is that C++ is a second branch of evolution making C++ more
compatible with C89 than C99.

The idea of Cake is to keep the main line of evolution of C and be always 100% 
compatible. Cake ♥ C.

The added extensions aims to keep the spirit of the language and implement proposed 
features in a way they can be experimented even before standardization.









 
 
  
 



