### Output

For a single input file:

`cake c:\project\file1.c`


```
    c:\project\
    ├── file1.c
    └── target\
        └── file1.c
```

or using -o option:

cake `c:\project\file1.c -o file89.c`

```
    c:\project\
    ├── file1.c
    └── file89.c
```

For multiple input files:


`cake c:\project\file1.c c:\project\other\file2.c`

Cake determines the common root path, in this case `c:\project\`, then recreates
the directory structure under target within this common path.

```
    c:\project\
    ├── file1.c
    ├── other\
    │   └── file2.c
    └── target\
        ├── file1.c
        └── other\
            └── file2.c
```

### C89 Backend

The current Cake backend generates C89-compatible source code. This output can
be passed directly to any C89 compiler (MSVC, GCC, Clang, etc.) to produce an
executable.

The generated output is a simplified, lowered form of the input. All modern C
constructs are removed and replaced with equivalent C89 idioms. Specifically,
the output contains:
    
    - No preprocessor directives
    - No typedef declarations (expanded inline)
    - No enum declarations (replaced with integer constants)
    - No const qualifiers
    - No constant expressions (evaluated at translation time)
    - No switch statements (lowered to equivalent constructs)
    - No nested struct/union definitions
    - No sizeof expressions (evaluated at translation time)
    - No local static variables (hoisted to file scope)
    - Array sizes are always explicit integer constants
    - Function prototypes are generated automatically

It currently requires bit-fields, but they are planned for removal.

