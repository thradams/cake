### Synopsis

`cake [options] source1.c [source2.c ...]`


### Examples

cake source.c
Compiles source.c and writes the C89 output to /[default-target]/source.c.

cake -target=X86_msvc source.c
Compiles source.c targeting MSVC x86 and writes output to /X86_msvc/source.c.

cake file.c -o file.cc && cl file.cc
Compiles file.c to file.cc, then passes file.cc to the MSVC compiler.