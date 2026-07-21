## Introduction

Cake is a source-to-source C compiler that translates modern C (C99 through C2Y)
into C89-compatible output, suitable for compilation by existing toolchains such
as GCC and MSVC.

Cake integrates directly into existing build environments:

- On **Windows**, it operates as an extension for MSVC. - On **Linux**, it
operates as an extension for GCC.

Where applicable, Cake uses the same command-line flags as GCC and MSVC to
minimize friction when integrating into existing projects.

