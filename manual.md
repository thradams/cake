# Cake C Compiler — Reference Manual

## 1. Introduction

Cake is a source-to-source C compiler that translates modern C (C99 through C2Y) into C89-compatible output, suitable for compilation by existing toolchains such as GCC and MSVC.

Cake integrates directly into existing build environments:

- On **Windows**, it operates as an extension for MSVC.
- On **Linux**, it operates as an extension for GCC.
- On **macOS**, it operates as an extension for Clang.

Where applicable, Cake uses the same command-line flags as GCC and MSVC to minimize friction when integrating into existing projects.

---

## 2. Getting Started

### 2.1 Platform Support

Cake runs on Windows (targeting MSVC), Linux (targeting GCC), and macOS (targeting Clang).
Cake can also perform cross-compilation, as long as you provide the headers from the target platform.

### 2.2 Include Directories

Include directories are specified in `cake.json`, a configuration file kept
next to the cake executable. It replaces the former `cakeconf.h` header.


**Discovering system include paths manually:**

On Windows (from a Visual Studio Developer Command Prompt):

```
echo %INCLUDE%
```

On Linux:

```
echo | gcc -E -Wp,-v -
```

On macOS:

```
echo | clang -v -E -
```

**Sample `cake.json` for Linux:**

```json
{
  "include_dirs": [
    "/usr/lib/gcc/x86_64-linux-gnu/11/include",
    "/usr/local/include",
    "/usr/include/x86_64-linux-gnu",
    "/usr/include"
  ]
}
```

**Sample `cake.json` for Windows:**

```json
{
  "include_dirs": [
    "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/include",
    "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/ATLMFC/include",
    "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/VS/include",
    "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/ucrt",
    "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/um",
    "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/shared",
    "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/winrt",
    "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt",
    "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um"
  ]
}
```

**Sample `cake.json` for macOS:**

```json
{
  "include_dirs": [
    "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/21/include",
    "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include",
    "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include"
  ]
}
```

Unlike the `cakeconf.h` it replaces, `cake.json` is data rather than a
preprocessed header: it cannot use `#ifdef` to hold several platforms in one
file, and it cannot `#include` another config. Each installation has its own
`cake.json`, describing the machine it runs on. To add include directories for
one project rather than the whole installation, use `-I` on the command line,
or `#pragma dir` in the source itself.

The cake IDE reads and writes this same file, keeping its own global compiler
options in it under a `"compile"` object. Both tools share one format, and
`-auto-config` rewrites only `"include_dirs"`, leaving anything else in the
file untouched.

### 2.3 Cake's Own Headers and `#include_next`

Cake ships its own directory of annotated standard headers (`stdlib.h`,
`string.h`, `stdio.h`, and the rest), carrying `_Owner`/`_Opt` and the other
ownership annotations described in 11.9. This directory is added to the
search path ahead of the real system include directories, so `#include
<stdlib.h>` resolves to Cake's annotated header first.

Cake's header then pulls in the real one with `#include_next <stdlib.h>` -
the same directive GCC and Clang use for this purpose - which continues the
search from the *next* directory after Cake's own, i.e. the real system
include path. This is how the annotated declarations and the platform's
actual declarations both end up in scope: Cake's header supplies the
contract, `#include_next` supplies everything else (types, other functions,
platform-specific details) from the real header.

This directory - `include` next to the cake executable - is built in: Cake
always searches it first, before every entry of `cake.json`'s
`"include_dirs"`, so it is not listed in `cake.json`.

The `-cake-headers` option switches to a different mode: it defines the
built-in macro `CAKE_HEADERS`, which each of Cake's headers checks to decide
whether to declare everything itself - types, macros, and function
prototypes - instead of deferring to `#include_next`. With `-cake-headers`,
Cake's headers are entirely self-contained and the real system headers are
never consulted at all, so compilation no longer depends on having a
system toolchain or its include paths configured. This is how Cake compiles
itself and runs its test suite portably; it is not meant for compiling
ordinary programs, since a program built this way only sees the subset of
the platform's declarations that Cake's own headers happen to provide.

### 2.4 System include paths auto-configuration

The `-auto-config` option fills in `cake.json`'s `"include_dirs"` automatically
by querying the active compiler environment:

```
cake -auto-config
```

On Windows this reads the `INCLUDE` variable, so it must be run from a Visual
Studio Developer Command Prompt. On Linux and macOS it reads the search list
reported by `gcc -v -E` / `clang -v -E`.

---

## 3. Invocation

### 3.1 Synopsis

```
cake [options] source1.c [source2.c ...]
```

### 3.2 Examples

```
cake source.c
```
Compiles `source.c` and writes the C89 output to `./<target>/source.c`, where `<target>` is the platform Cake was built for (e.g. `./macos_arm64/source.c`).

```
cake -target=x86_msvc source.c
```
Compiles `source.c` targeting MSVC x86 and writes output to `./x86_msvc/source.c`.

```
cake file.c -o file.cc && cl file.cc
```
Compiles `file.c` to `file.cc`, then passes `file.cc` to the MSVC compiler.

---

<a id="options"></a>

## 4. Command-Line Options

### 4.1 Preprocessor Options

**`-I <dir>`**  
Add `<dir>` to the list of directories searched for include files. Same as GCC and MSVC.

**`-D <macro>`**  
Define a preprocessing symbol for the source file. Same as GCC and MSVC.

**`-E`**  
Copy preprocessor output to standard output. Same as GCC and MSVC.

**`-H`**  
Output a list of include files used during compilation. Equivalent to `/showIncludes` in MSVC and `-H` in GCC.

**`-dump-tokens`**  
Output the token stream before preprocessing.

**`-preprocess-def-macro`**  
Expand and preprocess `#define` macros after expansion.


### 4.2 Output Options

**`-o <name.c>`**  
Set the output filename when compiling a single file. Same as GCC and MSVC.

**`-no-output`**  
Run all analysis passes but suppress file output.

**`-line-directives`**  
Emit `#line` directives in the generated C89 output to preserve source location information.

**`-sarif`**  
Generate SARIF diagnostic output files. Compatible with the [Microsoft SARIF Viewer](https://marketplace.visualstudio.com/items?itemName=WDGIS.MicrosoftSarifViewer) extension for Visual Studio.

**`-sarif-path <dir>`**  
Specify the directory for SARIF output. Typical Visual Studio External Tools invocation:  
`-w11 -msvc-output -no-output -sarif -sarif-path "$(SolutionDir).sarif" $(ItemPath)`

**`-msvc-output`**  
Format diagnostic output to be compatible with the Visual Studio IDE error parser. Same as `-fdiagnostics-format=msvc` plus `-fdiagnostics-color=never`.

**`-fdiagnostics-format=<format>`**  
Select how the position of each diagnostic is printed. Valid formats:

| Format | Output |
|---|---|
| `gcc` (default) | `file.c:1:2: warning 10: message` |
| `msvc` | `file.c(1,2): warning 10: message` |
| `ide` | `file.c:1:2: warning 10: message` |

Both shapes are understood by Visual Studio and by Visual Studio Code. The file being compiled is printed by name only; files reached through `#include` are printed with their full path.

**`-fdiagnostics-color=never`**  
Disable ANSI color codes in diagnostic output. Same as GCC.

### 4.3 Diagnostic Options

| Option | Effect |
|---|---|
| `-w<number>` | enable warning number `<number>`, e.g. `-w2`. See the [Warnings Reference](diagnostics.html) |
| `-wd<number>` | disable warning number `<number>`, e.g. `-wd2` |
| `-wall` / `-Wall` | enable all warnings |

**Disabled by default**  
Most warnings are on unless `-wd<number>` turns them off, but a few are off
until asked for:

| Number | Warning |
|---|---|
| `2` | unused variable |
| `6` | unused function parameter |
| `11` | style |
| `33` | nullable pointer flow check |
| `35` | nullable pointer flow check |
| `83` | parameter set but not used |
| `84` | variable set but not used |

**`-Werror`**  
Report every enabled warning as an error. Notes are not affected, and warnings that are disabled stay disabled. Because they become errors, warnings coming from included headers are no longer suppressed, and any occurrence makes the compilation fail.

**Per-line suppression with `lint` comments**  
Suppress a diagnostic on one line with a trailing `lint` comment listing its
number(s): `//lint 35`, `// lint 35`, or `/* lint 81 */`. An unnecessary
suppression is flagged with warning 59.


### 4.4 Target Options

**`-target=<name>`**  
Set the compilation target. Controls integer sizes, alignment, and the style of generated C89 output.

Available targets:

| Name | Platform | Output compiler |
|---|---|---|
| `default` | the platform Cake itself was built for (same as omitting `-target`) | — |
| `x86_x64_gcc` | Linux x86-64 | GCC |
| `x86_msvc` | Windows x86 | MSVC |
| `x64_msvc` | Windows x64 | MSVC |
| `macos_arm64` | macOS arm64 (Apple Silicon) | Clang |
| `catalina` | Catalina | Catalina C compiler |
| `ccu8` | Embedded / custom | — |

**`-auto-config`**  
Generate `cake.json` with the include directories of the current system.

### 4.5 Analysis Options

**`-fanalyzer`**  
Run Cake's built-in flow analysis pass, including ownership, nullability, and lifetime checks.

**`-const-literal`**  
Treat string literals as `const char[]` rather than `char[]`.

**`-unused-extern-report`**  
Track every non-static (external linkage) function across all the files given
in this invocation, and after the last one is compiled, report the ones that
were never called in any of them.

A `static` function's usage can already be decided by looking at its own file
alone, so it is reported immediately as each file is compiled (see warning 57
in the [Warnings Reference](diagnostics.html)). A non-static function can be
called from a different file than the one that defines it, so it needs the
whole set of files to be checked together; this option does that check and
prints its results once, at the end, instead of per file:

```
Functions never used in any of the compiled files:
  utils.c:42: warning: function 'legacy_helper' is not used in this build
```

This can only see the files passed to this invocation — a function only
called from a file outside this build (or from a different `cake` invocation
entirely) is still reported here. Functions declared in an `#include`d file
(system headers included) are never reported, even if unused, since they are
not part of the files given to this invocation. `main` is never reported
either, since it is the program's entry point and is not expected to be
called from within the program itself.

### 4.6 Style and Formatting Options

**`-style=<name>`**  
Select the coding style checked by diagnostic 11 (style). Passing `-style` turns diagnostic 11 on as a note; `-style=none` turns it off. Valid names: `none`, `cake`, `gnu`, `microsoft`.

**`-format`**  
Reformat the file's spacing, braces, `else` placement and indentation to match `-style` (defaults to `cake`) and print the result instead of compiling.

**`-format-lines=<first>:<last>`**  
Restrict `-format` to the given line range.

The built-in styles:

| | `cake` | `gnu` | `microsoft` |
|---|---|---|---|
| Function-body `{` | own line (Allman) | own line (Allman) | own line (Allman) |
| Control-flow `{` | own line (Allman) | own line (Allman) | own line (Allman) |
| `else` | new line | new line | new line |
| Pointer `*` | next to the type: `int* p` | next to the name: `int *p` | next to the name: `int *p` |
| Indentation | 4 spaces | tabs | 4 spaces |
| struct / enum names | snake_case | camelCase | PascalCase |
| Function names | snake_case | camelCase | PascalCase |
| Globals, locals, parameters, members | snake_case | snake_case | PascalCase |
| Enumerators | UPPERCASE | UPPERCASE | UPPERCASE |
| Spacing rules (below) | on | off | off |

In every style `}` must be on its own line. Indentation must be tabs only or spaces only, and with spaces a multiple of the indent width.

Spacing rules, checked when the style enables them:

| Rule | Example |
|---|---|
| one space after `,` | `f(a, b)` |
| no space before `;` | `return x;` |
| one space between a keyword and `(` | `if (x)` |
| no space between a callee and `(` | `f(x)` |
| one space on each side of a binary operator | `a + b` |
| one declarator per declaration | `int i; int j;` rather than `int i, j;` |

### 4.7 Using cake inside Visual Studio
Use cake as Custom Build Tool for a specific file.c

- Command line: `cake -sarif -sarif-path "$(SolutionDir).sarif" -line-directives -msvc-output file.c`
- Outputs: `x86/file.c`
- Add Output to item type: `C/C++ Compiler`

Then you can run/debug normally.

It also can be used as external tool for static analysis:

- Command: `cake.exe`
- Arguments: `-msvc-output -no-output -sarif -sarif-path "$(SolutionDir).sarif"  $(ItemPath)`
- Initial directory: `$(ItemDir)`
- [x] Use output window

### 4.8 Using cake inside Visual Studio Code
*TODO*


---

## 5. Output

### 5.1 C89 Backend

The current Cake backend generates C89-compatible source code. 
This output can be passed directly to any C89 compiler (MSVC, GCC, Clang, etc.) to produce an executable.

The generated output is a simplified, lowered form of the input. All modern C constructs are removed and 
replaced with equivalent C89 idioms. Specifically, the output contains:

- No preprocessor directives (except `#pragma pack`, see 5.4.13)
- No `typedef` declarations (expanded inline)
- No `enum` declarations (replaced with integer constants)
- No `const`, `restrict` or `register` (`volatile` is preserved)
- No constant expressions (evaluated at translation time)
- No `switch` statements (lowered to `if`/`goto`)
- No nested `struct`/`union` definitions (hoisted to file scope)
- No `sizeof` expressions (evaluated at translation time)
- No local `static` variables (hoisted to file scope)
- No local declarations with initializers (initialization becomes assignment)
- No character constants (replaced with integer constants)
- Array sizes are always explicit integer constants
- Function prototypes are generated automatically
- Unused `static` objects, unused functions with internal linkage, and unused type definitions are not emitted

It currently requires bit-fields, but they are planned for removal.

The exact subset of C accepted in the output is given as a grammar in Appendix A.
Anyone writing a new backend (or a tool that consumes Cake output) only needs to handle that subset.

### 5.2 Output Files

For a single input file:

```
cake c:\project\file1.c
```

```
c:\project\
├── file1.c
└── x64_msvc\
    └── file1.c
```

The output directory is named after the compilation target (see `-target`, section 4.4): `x64_msvc`, 
`x86_x64_gcc`, `macos_arm64`, etc. The examples below use `x64_msvc`.

Using the `-o` option (single input file only):

```
cake c:\project\file1.c -o file89.c
```

```
c:\project\
├── file1.c
└── file89.c
```

For multiple input files:

```
cake c:\project\file1.c c:\project\other\file2.c
```

Cake determines the common root path, in this case `c:\project\`,
then recreates the directory structure under `<target>` within this common path.

```
c:\project\
├── file1.c
├── other\
│   └── file2.c
└── x64_msvc\
    ├── file1.c
    └── other\
        └── file2.c
```

### 5.3 Structure of a Generated File

Every generated file follows the same layout:

```c
/* Cake 0.14.41 macos_arm64 2026-09-13 08:38:33 */      /* 1. header comment */

struct point {                                           /* 2. struct/union definitions */
    int x;
    int y;
};

static void * __cmemset(void *dest, int ch, unsigned long count);  /* 3. helper prototypes */

static int __ccalls = 0;                                 /* 4. hoisted local statics */

int sum(int n)                                           /* 5. declarations and definitions, */
{                                                        /*    in source order              */
    ...
}

int printf(char *, ...);                                 /*    prototypes for functions used */
int sum(int n);                                          /*    by the next definition        */

int main(void)
{
    ...
}

static void * __cmemset(void *ptr, int value, unsigned long count)  /* 6. helper definitions */
{
    ...
}
```

1. **Header comment** — Cake version, target name and generation timestamp.
2. **Type definitions** — every `struct`/`union` that is used, hoisted to file scope and emitted in dependency 
   order (a member's type is always defined before the aggregate that contains it). Definitions of packed or 
   aligned types carry the target-specific spelling (5.4.13).
3. **Helper prototypes** — declarations of the runtime helpers the file needs (5.6). Only helpers actually used are emitted.
4. **Hoisted statics** — local `static` objects, local functions, function literals and `__func__` strings, renamed to file scope.
5. **Declarations and definitions** — file-scope objects and functions in source order. Before each function 
   definition Cake emits prototypes for every function that definition calls and every file-scope object it 
   uses that has not been declared yet, so the output never depends on a header.
6. **Helper definitions** — the bodies of the helpers declared in step 3.

Inside every function, all local declarations appear at the top of their block (without initializers), 
followed by a blank line, followed by the statements. Initialization is turned into assignments.

### 5.4 Lowering Reference

This section shows how each input construct appears in the output.

#### 5.4.1 Preprocessor, constants and constant expressions

Macros are expanded, `#include` is resolved, and every constant expression is evaluated by Cake. 
`sizeof`, `_Countof`, `_Alignof`, enumeration constants, `constexpr` objects, `const` objects with a known 
value, character constants and arithmetic on constants all become plain integer or floating literals.

```c
enum color { RED, GREEN = 5 };
const double pi = 3.14;
#define SIZE 4
int x = GREEN + sizeof(int) * SIZE + 'a' + (int)pi + 0x10 + 0b11;
```

```c
int x = 5 + 4UL * 4 + 97 + 3 + 16 + 3;   /* the whole expression is folded when it is constant */
```

Integer literals are printed in decimal with the suffix of their type (`U`, `L`, `UL`, `LL`, `ULL`). 
`sizeof` produces a `size_t` literal (`8UL` on LP64 targets, `8ULL` on Windows x64). Floating literals are 
printed in decimal with `f` or `L` suffix when needed (`0x1.8p1` → `3.0`). `nullptr` becomes `0`. 
`true`/`false` become `1`/`0`. `u8"..."` becomes a plain string; `L"..."` is preserved.

`static_assert`, `_Static_assert` and `_Assert` are checked by Cake and produce no output.

#### 5.4.2 typedef, bool, auto, typeof, _BitInt

`typedef` names are replaced by the type they denote. `bool` is `unsigned char`. `auto` and `typeof` are 
resolved to the deduced type. `_BitInt(N)` is lowered to the smallest standard integer that holds `N` bits, 
and every store wraps the value to `N` bits.

```c
typedef struct point { int x, y; } point_t;
point_t p = {.x = 1, .y = 2};
bool ok = true;
auto k = 2;
_BitInt(12) b = 100wb;
```

```c
struct point p;
unsigned char ok;
int k;
short b;

p.x = 1;
p.y = 2;
ok = 1;
k = 2;
b = ((short)(((long long)((unsigned long long)(100) & 0xfffULL) ^ 0x800LL) - 0x800LL));
```

#### 5.4.3 Local declarations and initializers

Local declarations are moved to the top of their block and lose their initializer. Scalars are 
assigned; aggregates are assigned member by member; `{}`, `{0}` and partially initialized aggregates are 
zeroed with `__cmemset`; `char` arrays initialized from a string literal use `__cmemcpy`.

```c
int arr[4] = {};
int a[2][2] = {{1}, {2}};
char s[10] = "abc";
struct point c = {.y = 5};
```

```c
int arr[4];
int a[2][2];
char s[10];
struct point c;

__cmemset(&arr, 0, 16);
a[0][0] = 1;
a[0][1] = 0;
a[1][0] = 2;
a[1][1] = 0;
__cmemcpy(s, "abc", 10);
c.x = 0;
c.y = 5;
```

File-scope initializers are preserved but flattened: designators are resolved and nested braces removed, 
so each initializer is a single flat, brace-enclosed list of constants.

```c
static struct s table[2] = { {1, 2, "one"}, {3, 4, "two"} };
int arr2[2][3] = { {1, 2, 3}, {4, 5, 6} };
char greeting[] = "hi\n";
```

```c
static struct s table[2] = {1,2,"one",3,4,"two"};
int arr2[2][3] = {1,2,3,4,5,6};
char greeting[4] = "hi\n";
```

#### 5.4.4 Local static objects

Local `static` objects are hoisted to file scope and renamed with the `__c` prefix.

```c
int sum(int n) { static int calls = 0; return ++calls; }
```

```c
static int __ccalls = 0;

int sum(int n)
{
    return ++__ccalls;
}
```

#### 5.4.5 Declarations in `for` and `if`/`switch`

A `for` with a declaration, and `if`/`switch` with an init-statement, are wrapped in a block that holds the declaration.

```c
for (int i = 0; i < 4; i++) { total += i; }
if (int v = f(); v > 5) { m += v; }
```

```c
{
    int i;

    i = 0;
    for (; i < 4; i++)
    {
        total += i;
    }
}
{
    int v;

    v = f();
    if (v > 5)
    {
        m += v;
    }
}
```

#### 5.4.6 `switch`

`switch` is lowered to a block containing a temporary, a chain of `if`/`goto` dispatches, the labeled body 
and an exit label. `break` inside the switch becomes `goto` to the exit label. Case ranges become range tests. 
`[[fallthrough]]` needs no output.

```c
switch (n) {
    case 1: total += 8; break;
    case 2 ... 5: total += 12; [[fallthrough]];
    default: total += 1;
}
```

```c
/*switch*/
{
    int __a = n;
    if (__a == 1) goto __L1; /*case 1*/
    if (__a >= 2 && __a <= 5) goto __L2; /*case 2 ... 5*/
    goto __L3; /* default */

    {
        __L1: /*case 1*/ 
        total += 8;
        goto __L0; /* break */

        __L2: /*case 2 ... 5*/ 
        total += 12;
        __L3: /* default */ 
        total += 1;
    }
    __L0:;
}
```

`break` and `continue` inside loops are preserved as-is.

#### 5.4.7 Nested and anonymous `struct`/`union`

Nested definitions are hoisted to file scope. Anonymous structs and unions receive a generated tag 
(`__tagN`) and the anonymous member a generated name (`__mN`); accesses are rewritten accordingly.

```c
struct outer {
    struct inner { int a; } in;
    union { int u; float f; };
};
o.u = 1;
```

```c
struct inner {
    int a;
};

union __tag10 {
    int u;
    float f;
};

struct outer {
    struct inner in;
    union __tag10 __m0;
};

o.__m0.u = 1;
```

#### 5.4.8 Variable-length arrays

Each variably-modified size is captured in a `__vmN` variable at the point of declaration and the storage is 
obtained with the target's `alloca` (`__builtin_alloca` for GCC/Clang targets, `_alloca` for MSVC targets). 
`sizeof`/`_Countof` on a VLA read the `__vmN` variable.

```c
int vla[n];
vla[0] = 1;
o.u = _Countof(vla);
```

```c
unsigned long __vm0;
int * vla;

__vm0 = n;
vla = __builtin_alloca(4 * __vm0);
vla[0] = 1;
o.u = __vm0;
```

#### 5.4.9 `_Defer`, `try`/`catch`/`throw`

The body of `_Defer` is copied, as a block, before every exit from the enclosing scope (`return`, `break`, 
`continue`, `goto`, falling off the end). When the exit is a `return` with an expression, the expression is 
first stored in a temporary (`__b`). `try` becomes `if (1)`, `catch` becomes `else` with a label, and 
`throw` becomes `goto` to that label.

```c
int *p = malloc(n);
_Defer { free(p); }
if (p == NULL) return -1;
try {
    if (n > 10) throw;
    p[0] = 2;
}
catch {
    return -2;
}
return p[0];
```

```c
p = malloc(n);
if (p == 0)
{
    {
        free(p);
    }
    return -1;
}
if (1) /*try*/
{
    if (n > 10)
    {
        goto __L1; /* throw */
    }
    p[0] = 2;
}
else __L1: /*catch*/ 
{
    {
        free(p);
    }
    return -2;
}
__b = p[0];
{
    free(p);
}
return __b;
```

#### 5.4.10 `_Generic`, compound literals, statement expressions, Elvis operator

`_Generic` is resolved at translation time and only the selected expression is emitted. A compound literal 
becomes a temporary (`__a`) declared at the top of the block and initialized by assignment. A statement 
expression becomes a block whose last expression is stored in a temporary. `a ?: b` is rewritten to the 
equivalent conditional (or folded when `a` is constant).

```c
int r = _Generic(x, int: 1, default: 2);
int *cl = (int[]){1, 2, 3};
int m = ({ int t = add(1, 2); t * 2; });
int e = 0 ?: 7;
```

```c
int r;
int * cl;
int __a[3];
int m;
int __a;
int e;

r = 1;
__a[0] = 1;
__a[1] = 2;
__a[2] = 3;
cl = __a;
{
    int t;

    t = add(1, 2);
    __a = t * 2;
}
m = __a;
e = 7;
```

#### 5.4.11 Local functions, function literals and `__func__`

Local functions and function literals become file-scope `static` functions with a `__c` prefix (`__cfn` 
for literals). `__func__` becomes a file-scope `static char` array named `__cake_func_<function>`.

```c
double g(void)
{
    int (*sq)(int) = (static int (int x)) { return x * x; };
    static int add(int a, int b) { return a + b; }
    puts(__func__);
    return sq(3) + add(1, 2);
}
```

```c
static int __cfn(int x);
static int __cadd(int a, int b);
static char __cake_func_g[] = "g";

double g(void)
{
    int (* sq)(int);

    sq = __cfn;
    puts(__cake_func_g);
    return sq(3) + __cadd(1, 2);
}

static int __cfn(int x)
{
    return x * x;
}

static int __cadd(int a, int b)
{
    return a + b;
}
```

#### 5.4.12 Qualifiers, storage classes, parameters

`const`, `restrict`, `register`, `inline`, `_Noreturn`, `constexpr` and attributes are dropped. `volatile` on 
an object's type is preserved (`volatile int v;`), but qualifiers on pointers (`int * volatile p`) are dropped. 
`static` and `extern` are preserved. Unnamed parameters in definitions receive no name in prototypes 
(`int printf(char *, ...);`). Old-style and prototype-less declarations never appear: every function 
declarator has a full parameter list, and `(void)` is used for functions without parameters.

#### 5.4.13 Packed and aligned structures

Alignment information is the only thing that cannot be expressed in plain C89, so it is emitted in the 
dialect of the target compiler:

| Input | GCC / Clang targets | MSVC targets |
|---|---|---|
| `#pragma pack(push, n)` ... `#pragma pack(pop)` around a struct | `#pragma pack(push, n)` / `#pragma pack(pop)` around the definition | same |
| `struct __attribute__((packed)) S` | `#pragma pack(push, 1)` / `#pragma pack(pop)` around the definition | same |
| `__attribute__((packed))` on a single member | `__attribute__((packed))` after the member | dropped (MSVC has no per-member spelling) |
| `struct __attribute__((aligned(n))) S` / `_Alignas(n)` | `struct __attribute__((aligned(n))) S {` | `__declspec(align(n)) struct S {` |
| `__cdecl`, `__stdcall`, `__fastcall` | dropped | preserved on the declarator |

```c
#pragma pack(push, 1)
struct packed { char c; int i; };
#pragma pack(pop)
struct __attribute__((aligned(16))) al { char c; };
```

```c
#pragma pack(push, 1)
struct packed {
    char c;
    int i;
};
#pragma pack(pop)

struct __attribute__((aligned(16))) al {       /* __declspec(align(16)) struct al { on MSVC targets */
    char c;
};
```

Because the `#pragma pack` pair always encloses exactly one type definition, a consumer of the output can 
treat it as a prefix/suffix of that definition rather than as general preprocessor state.

### 5.5 Generated Names

All generated identifiers start with a double underscore. A backend or a consumer of the output should 
never rely on their exact numbering, only on the prefixes.

| Prefix | Meaning |
|---|---|
| `__c<name>` | hoisted local `static` object or local function (`__ccalls`, `__cadd`) |
| `__cfn` | hoisted function literal |
| `__cake_func_<function>` | `__func__` string of `<function>` |
| `__L<n>` | label generated for `switch`, `throw`, `catch` |
| `__a` | temporary for a `switch` controlling expression, a compound literal or a statement expression |
| `__b` | temporary holding a `return` value while `_Defer` blocks run |
| `__vm<n>` | captured size of a variably-modified type |
| `__tag<n>` | tag given to an anonymous `struct`/`union` |
| `__m<n>` | name given to an anonymous member |
| `__cmemset`, `__cmemcpy` | runtime helpers (5.6) |

Cake also emits comments (`/*switch*/`, `/*case 1*/`, `/* break */`, `/*try*/`, `/*catch*/`, 
`/* throw */`, `/* default */`) to make the lowered control flow readable. They carry no meaning.

### 5.6 Runtime Helpers

The output is self-contained: it never includes a header. When a construct needs library support, Cake 
emits a `static` helper with a prototype at the top of the file and a definition at the end.

| Helper | Emitted when |
|---|---|
| `static void * __cmemset(void *dest, int ch, size_t count)` | an aggregate is zero-initialized (`{}`, `{0}`, partial initializers) |
| `static void * __cmemcpy(void *dest, const void *src, size_t n)` | a `char` array is initialized from a string literal |
| `static void __assert_fail(...)` / `static void __assert_rtn(...)` | `assert()` from the system header is used and the target's libc declares that symbol |

`size_t` is spelled with the target's underlying type (`unsigned long` on LP64, `unsigned long long` on 
Windows x64, `unsigned int` on 32-bit targets).

Functions from the standard library that the program calls (`printf`, `malloc`, `free`, `strlen`, ...) are 
not helpers: Cake emits their prototypes, with `const` removed, and the C89 compiler links them from libc.

### 5.7 Compiling the Output

The generated file is a complete translation unit. It can be compiled with the target compiler with no 
extra flags:

```bash
gcc -w x86_x64_gcc/file1.c -o file1
```

Warnings are disabled here only because prototypes for libc functions are emitted without `const` 
(`int puts(char *)`), which some compilers flag when they also see the system header.

---

## 6. Language Standard Support

Standard reference documents:

- **C89:** https://nvlpubs.nist.gov/nistpubs/Legacy/FIPS/fipspub160.pdf
- **C99:** https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
- **C11:** https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
- **C23:** https://open-std.org/JTC1/SC22/WG14/www/docs/n3096.pdf
- **C2Y:** https://www.open-std.org/JTC1/SC22/WG14/www/docs/n3854.pdf

---

## 7. C99 Features

### 7.1 `restrict` Pointers

`restrict` is parsed and type-checked but stripped from the generated C89 output.

<!-- runnable -->

```c
void f(const char* restrict s);
int main() { f(""); }
```


### 7.2 Variably-Modified (VM) Types and Variable-Length Arrays

**VM type pointer:**

<!-- runnable -->

```c
#include <stdlib.h>
#include <stdio.h>

int main() {
    int n = 2, m = 3;
    int (*p)[n][m] = malloc(sizeof *p);
    printf("%zu\n", sizeof(*p));
    free(p);
}
```


**VLA as 2D function parameter:**

<!-- runnable -->

```c
#include <stdio.h>

void print_matrix(int rows, int cols, int m[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            printf("%d ", m[i][j]);
        printf("\n");
    }
}

int main(void) {
    int r = 2, c = 3;
    int m[r][c];
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            m[i][j] = i * c + j;
    print_matrix(r, c, m);
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n683.htm

### 7.3 Flexible Array Members

<!-- runnable -->

```c
#include <stdio.h>
#include <stdlib.h>

struct X {
    int count;
    double values[];   /* flexible array member */
};

int main() {
    int n = 3;
    struct X* p = malloc(sizeof(struct X) + n * sizeof(double));
    p->count = n;
    p->values[0] = 10.0;
    p->values[1] = 20.0;
    p->values[2] = 30.0;
    for (int i = 0; i < p->count; ++i)
        printf("%f\n", p->values[i]);
    free(p);
}
```


The size of a structure with a flexible array member is computed as if the member were omitted, except that additional trailing padding may be present.

### 7.4 `static` and Type Qualifiers in Array Declarators

The `static` keyword in array declarators is parsed and checked. Passing `NULL`, `nullptr`, or an array smaller than the declared minimum is a diagnostic.

<!-- runnable -->

```c
#include <stdlib.h>

void F(int a[static 5]) {}

int main() {
    F(0);              /* error: null pointer */
    F(NULL);           /* error: null pointer */
    int a[] = {1, 2, 3};
    F(a);              /* error: fewer than 5 elements */
    int b[] = {1, 2, 3, 4, 5};
    F(b);              /* ok */
}
```


### 7.5 Hexadecimal Floating Constants

```c
double d = 0x1p+1;
```

Cake has no way to emit a hexadecimal floating constant in C89 output, so it
converts the literal to a decimal one:

```c
double a = 0x1p+1;                    /* becomes  a = 2.0;    */
double b = 0x1.5555555555555p-2;      /* becomes  b = 0.33333333333333331; */
double c = 0x1p-1074;                 /* becomes  c = 4.9406564584124654e-324; */
```

For `float` and `double` the conversion is **lossless**: the decimal Cake writes
is guaranteed to read back as the exact same value, bit for bit. The same holds
for ordinary decimal constants, which are re-emitted in normalized form - and
because the form chosen is the *shortest* one that round-trips, the output stays
readable:

```c
double g = 0.1;      /* stays   g = 0.1;    not 0.10000000000000001 */
float  f = 0.1f;     /* stays   f = 0.1f;   not 0.100000001490116119384765625 */
double h = 1.0/3.0;  /* becomes h = 0.3333333333333333; */
```

The printer is an implementation of **Grisu2**:

> Florian Loitsch, *Printing Floating-Point Numbers Quickly and Accurately with
> Integers*, PLDI 2010. <https://doi.org/10.1145/1806596.1806623>
> - [paper (PDF)](https://www.cs.tufts.edu/~nr/cs257/archive/florian-loitsch/printf.pdf)

Useful cross-references when reading `cake_dtoa_shortest` in `src/object.c`:

- RapidJSON, the same variant, well commented:
  <https://github.com/Tencent/rapidjson/blob/master/include/rapidjson/internal/dtoa.h>
- The author's reference implementation, Grisu2 and Grisu3:
  <https://github.com/google/double-conversion>

Grisu2 guarantees the round-trip but not minimality: for roughly one value in a
thousand it emits one digit more than strictly necessary. It never emits a wrong
one. Guaranteed-shortest output needs Grisu3 or Ryu, which both require a bignum
fallback path.

The one case where precision *can* be lost is `long double` on targets where it
is wider than 64 bits (the 80-bit x87 format gcc and clang use on x86). There
Cake falls back to the host's `snprintf`, and the value has in any case already
passed through the host's own `long double`. Do not rely on the exact value of a
`long double` constant beyond `double` precision.


#### Infinity and NaN

Infinity and NaN have no literal form in C. When a constant expression folds to
one of them, Cake emits the same construct the standard headers use - an
overflowing product, cast to the wanted type:

```c
double d = INFINITY;    /* becomes  d = ((float)(1e+300 * 1e+300)); */
double n = NAN;         /* becomes  n = ((float)((1e+300 * 1e+300) * 0.0)); */
```

### 7.6 Compound Literals

<!-- runnable -->

```c
struct s { int i; };

int f(void) {
    struct s *p = 0, *q;
    int j = 0;
    again:
        q = p, p = &((struct s){ j++ });
    if (j < 2) goto again;
    return p == q && q->i == 1;
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n716.htm

### 7.7 Designated Initializers

<!-- runnable -->

```c
int main() {
    int a[6] = {[4] = 29, [2] = 15};
    struct point { int x, y; };
    struct point p = { .y = 2, .x = 3 };
}
```

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n494.pdf

### 7.8 Declarations in `for` Loop Initializers

<!-- runnable -->

```c
int main() {
    const int max = 10;
    for (int n = max - 1; n >= 0; n--) {
        /* ... */
    }
}
```


### 7.9 `inline` Functions

<!-- runnable -->

```c
inline int sum(int a, int b) { return a + b; }

int main(void) {
    int r = sum(1, 2);
}
```


Cake does not perform function inlining itself; what it does with `inline`
depends on whether the translation unit defines the function:

- An `inline` function **defined** in the unit is emitted as `static`, both
  its prototype and its body, so every unit that includes the header gets
  its own copy and nothing is exported. The example above generates
  `static int sum(int a, int b)`.
- An `inline` **prototype with no definition** in the unit is a reference to
  a function defined somewhere else, so it is emitted as an ordinary
  external prototype. Library headers use this form - Cake's own
  `ownership.h` declares `inline char* strrchr(...)` to attach its
  annotations to the C library function - and emitting it as `static` would
  leave the host compiler with a static function that is declared but never
  defined.

This rule is the same for every target; on MSVC an inline function with a
body in the unit (the `snprintf` family in its `stdio.h`, for instance) is a
definition and takes the first form.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n709.htm

### 7.10 `_Pragma` Preprocessing Operator

```c
#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)
LISTING(..listing.dir)
```

### 7.11 `__func__` Predefined Identifier

<!-- runnable -->

```c
#include <stdio.h>
int main() {
    printf("%s\n", __func__);
}
```



### 7.12 Variadic Macros

<!-- runnable -->

```c
#include <stdio.h>
#define debug(...) fprintf(stderr, __VA_ARGS__)

int main() {
    debug("X = %d\n", 1);
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n707.htm

### 7.13 `_Bool`

<!-- runnable -->

```c
int main(void) {
    _Bool b = 1;
}
```


### 7.14 Line Comments

C99 `//` line comments are implemented. 

### 7.15 Complex and Imaginary Types

**Not implemented.**

### 7.16 Universal Character Names (`\u`, `\U`)

**Not implemented.**

---

## 8. C11 Features

### 8.1 `_Static_assert` / `static_assert`

<!-- runnable -->

```c
int main() {
    _Static_assert(1 == 1, "error");
}
```


`_Static_assert` is aliased to `static_assert` in C23.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1330.pdf

### 8.2 Anonymous Structures and Unions

<!-- runnable -->

```c
struct v {
    union {
        struct { int i, j; };       /* anonymous struct */
        struct { long k, l; } w;
    };
    int m;
} v1;

int main() {
    v1.i = 2;    /* valid */
    v1.w.k = 5;  /* valid */
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1406.pdf

### 8.3 `_Noreturn`

```c
_Noreturn void f(void) {
    abort();
}
```

`_Noreturn` became `[[noreturn]]` in C23.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1478.htm

### 8.4 `_Thread_local` / `_Atomic`

`_Thread_local` is supported and maps to `__declspec(thread)` in MSVC output and `__thread` in GCC output.

`_Atomic` is **not implemented**.

### 8.5 `_Generic` Type-Generic Expressions

<!-- runnable -->

```c
#include <math.h>

#define cbrt(X) _Generic((X),    \
                  double: cbrtl, \
                  float: cbrtf,  \
                  default: cbrtl \
              )(X)

int main(void) { cbrt(1.0); }
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1441.htm

### 8.6 Unicode Character Constants (`u''`, `U''`)

<!-- runnable -->

```c
int i  = U'ç';
int i2 = u'ç';
```


> **Note:** Cake assumes source files are UTF-8 encoded.

### 8.7 UTF-8 String Literals (`u8"..."`)

<!-- runnable -->

```c
char* s1 = u8"maçã";
char* s2 = u8"maca";
```


> **Note:** Cake assumes source files are UTF-8 encoded.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1488.htm

### 8.8 `_Alignof` / `alignof`

<!-- runnable -->

```c
int main() {
    int align = alignof(int);
}
```


`_Alignof` became `alignof` in C23.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1397.htm

### 8.9 `_Alignas` / `alignas`

Uses `__declspec(align(n))` in MSVC output and `__attribute__((aligned(n)))` in GCC output.

---

## 9. C23 Features

### 9.1 Variably-Modified (VM) Types — Mandatory

C23 formally separates two concepts previously bundled in C99:

**VLA objects** (`int a[n]`) remain **optional** in C23. 
Implementations defining `__STDC_NO_VLA__` do not support them. 

Cake implements VLA, and it is allocated on the stack using `alloca`. 
The use of VLA is discouraged.


**VM types** (`int (*p)[n]`) are **mandatory** in all conforming C23 implementations. 
Cake supports VM type pointers and translates them to C89-compatible output.


<!-- runnable -->

```c
/* VM type pointer - mandatory in C23 */
void foo(int n, double (*x)[n]) {
    (*x)[0] = 1.0;
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2778.pdf

### 9.2 `static_assert` — Single-Argument Form

<!-- runnable -->

```c
int main(void) {
    static_assert(1 == 2);   /* no message argument required */
}
```


### 9.3 `u8` Character Prefix

<!-- runnable -->

```c
int main() {
    unsigned char c = u8'~';
}
```


Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf

### 9.4 No Function Declarations Without Prototypes

In C23, calling an undeclared function is a constraint violation:

<!-- runnable -->

```c
int main() {
    func();   /* error in C23 */
}
```


Reference: https://www.open-std.org/JTC1/SC22/WG14/www/docs/n2841.htm

### 9.5 Unnamed Parameters in Function Definitions

<!-- runnable -->

```c
int f(int);

int f(int) {   /* unnamed parameter is valid */
    return 0;
}
```


> **Note:** TODO add placeholder name in the C89 output.

Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n2480.pdf

### 9.6 Digit Separators

<!-- runnable -->

```c
int main() {
    int a = 1000'00;
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2626.pdf

### 9.7 Binary Literals

<!-- runnable -->

```c
#define X  0b1010

int main() {
    int a = X;
    int b = 0B1010;
}
```


### 9.8 `nullptr` Constant

<!-- runnable -->

```c
int main() {
    void* p          = nullptr;
    auto  p2         = nullptr;
    typeof(nullptr) p3 = nullptr;
}
```


Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n3042.htm

### 9.9 `true` and `false` as First-Class Keywords

<!-- runnable -->

```c
int main() {
    bool b = true;
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2935.pdf

### 9.10 Empty Initializer `{}`

<!-- runnable -->

```c
int main() {
    struct X { int i; } x = {};
    x = (struct X){};
}
```


> **Note** Empty initializer can be used initialize VLAs

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2900.htm

### 9.11 `auto` Type Deduction

<!-- runnable -->

```c
static auto a = 3.5;
auto p = &a;

double A[3] = { 0 };
auto pA = A;
auto qA = &A;
```


Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n3007.htm

### 9.12 `typeof` / `typeof_unqual`

<!-- runnable -->

```c
#define SWAP(a, b) \
    do { typeof(a) temp = a; a = b; b = temp; } while (0)

int main() {
    int a = 1;
    typeof(a) b = 1;
    typeof(int*) p1, p2;
    SWAP(a, b);
}
```


Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n2927.htm

### 9.13 `constexpr`

<!-- runnable -->

```c
#include <stdio.h>

constexpr int c     = 123;
constexpr int c2    = c + 1000;
constexpr double PI = 3.14;

int a[c];
static_assert(PI + 1 == 3.14 + 1.0);

int main() {
    constexpr char ch = 'a';
    printf("%f %c", PI, ch);
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3018.htm

### 9.14 Improved and enhancements to Enumerations (Typed Enums)

<!-- runnable -->

```c
enum X : short { A };

int main() {
    enum X x = A;
}
```


Reference: 
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm

Improved enumerations
Reference: 
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3029.htm

<!-- runnable -->

```c
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3029.htm

enum a {
	a0 = 0xFFFFFFFFFFFFFFFFULL
};

int main () {}
```


### 9.15 Attributes (`[[...]]`)

Cake supports the C23 double-bracket attribute syntax. Recognized standard attributes:

**`[[fallthrough]]`** *(Supported)*  
Warns (`-w88`) about a `case`/`default` label reached by falling through a statement that doesn't end with `break`/`return`/`continue`/`goto`. Write `[[fallthrough]];` as the last statement before the label to mark it intentional; grouped empty labels (`case 1: case 2:`) never warn. The attribute itself must be immediately followed by a `case`/`default` label, or it is flagged as misplaced.

<!-- runnable -->

```c
void g(void);
void h(void);

void f(int n) {
    switch (n) {
    case 1:
    case 2:          /* grouped labels, no warning */
        g();
        [[fallthrough]];
    case 3:
        h();
        break;
    case 4:
        g();          /* warning: unannotated fall-through */
    case 5:
        break;
    }
}

int main(void) {}
```

**`[[deprecated]]`** *(Supported)*  
Emits a warning when the annotated entity is used. Compile with `-w03`.


<!-- runnable -->

```c
[[deprecated]] void f2(void) {}
struct [[deprecated]] S { int a; };

int main(void) {
    struct S s;
    f2();         /* warning: deprecated */
}
```



**`[[maybe_unused]]`** *(Supported)*  
Suppresses unused-variable or unused-parameter diagnostics.

```c
void f([[maybe_unused]] int arg1, int arg2) {}
```

**`[[nodiscard]]`** *(Supported — optional message argument not yet implemented)*  
Emits a warning when the return value of the annotated function is discarded.

<!-- runnable -->

```c
struct [[nodiscard]] error_info { int error; };
struct error_info enable_safety(void);

void test(void) {
    enable_safety();   /* warning: return value discarded */
}
```



**`[[noreturn]]`** *(Supported)*  
Replaces C11 `_Noreturn`.

**`[[unsequenced]]`** / **`[[reproducible]]`** *(TODO)*

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2335.pdf

### 9.16 `__has_attribute` / `__has_include`

<!-- runnable -->

```c
#if __has_c_attribute(fallthrough)
#  warning Attribute supported
#endif

#if __has_include(<stdio.h>)
#  warning Header found
#endif
```

### 9.17 `#warning` Directive

<!-- runnable -->

```c
int main() {
#warning my warning message
}
```




Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2686.pdf

### 9.18 `#embed` Directive

`#embed` expands to the bytes of a resource as a comma-separated list of
integer constants.

<!-- runnable -->

```c
#include <stdio.h>

int main() {
    static const char file_txt[] = {
#embed "stdio.h" limit(64)
        , 0
    };
    printf("%s\n", file_txt);
}
```

The resource is searched like an `#include`: the `"file"` form looks in the
directory of the current file first and then in the include directories; the
`<file>` form looks only in the include directories.

The standard parameters are supported (also with the `__limit__` spelling):

| Parameter | Effect |
|---|---|
| `limit(N)` | at most `N` bytes; `N` is a constant expression evaluated as in `#if` (`defined` is not allowed, negative values are an error) |
| `prefix(tokens)` | emitted before the bytes, only if the resource is not empty |
| `suffix(tokens)` | emitted after the bytes, only if the resource is not empty |
| `if_empty(tokens)` | emitted instead of the bytes when the resource is empty (or `limit(0)`) |

Each standard parameter can appear at most once; an unknown parameter is an
error. `vendor::parameter` names are accepted and ignored.

```c
static const char message[] = {
#embed "message.txt" suffix(, '\0') if_empty('n', 'o', 'n', 'e', '\0')
};
```

`__has_embed(resource parameters)` can be used in `#if` and evaluates to
`__STDC_EMBED_NOT_FOUND__` (0), `__STDC_EMBED_FOUND__` (1) or
`__STDC_EMBED_EMPTY__` (2). It also evaluates to 0 when a parameter is not
supported.

```c
#if __has_embed("logo.png") == __STDC_EMBED_FOUND__
static const unsigned char logo[] = {
#embed "logo.png"
};
#endif
```

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3017.htm

### 9.19 `#elifdef` / `#elifndef`

<!-- runnable -->

```c
#define Y

#ifdef X
#  define VERSION 1
#elifdef Y
#  define VERSION 2
#else
#  define VERSION 3
#endif
```


### 9.20 `__VA_OPT__`

<!-- runnable -->

```c
#define F(...)    f(0 __VA_OPT__(,) __VA_ARGS__)
#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)

void f(int i, ...) {}

int main() {
    int a = 1, b = 2, c = 3;
    F(a, b, c);
    F();
    G(a, b, c);
    G(a);
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3033.htm

### 9.21 `_BitInt(N)` — Bit-Precise Integers

Supported for widths up to 64 bits (`BITINT_MAXWIDTH` is 64). Wider types are
reported as an error.

<!-- runnable -->

```c
typedef unsigned _BitInt(24) u24;

struct S {
    _BitInt(12) m;
    unsigned _BitInt(3) n : 2;
};

int main() {
    _BitInt(12) a = 100;
    unsigned _BitInt(12) b = 4095uwb;
    signed _BitInt(40) c = -1;
    u24 d = 0xFFFFFF;

    static_assert(sizeof(_BitInt(12)) == 2);
    static_assert(_Generic(a + a, _BitInt(12): 1, default: 2) == 1);

    constexpr unsigned _BitInt(12) w = (unsigned _BitInt(12)) 4096;
    static_assert(w == 0);
}
```

The front end follows C23:

- the width is part of the type (`_BitInt(12)` and `_BitInt(13)` are different types, `_Generic` tells them apart);
- bit-precise types are not subjected to integer promotion, so `a + a` keeps the type `_BitInt(12)`;
- the conversion rank is just below the first standard type that holds `N` bits, and grows with the width, so `_BitInt(12) + 1` is `int` while `_BitInt(40) + 1` is `_BitInt(40)`;
- the `wb` / `uwb` suffixes give literals the narrowest bit-precise type that holds the value (`255uwb` is `unsigned _BitInt(8)`, `1wb` is `_BitInt(2)`);
- constant expressions wrap at `N` bits, so `(unsigned _BitInt(12)) 4096` is `0`;
- `signed _BitInt(1)`, `_BitInt(0)` and widths above 64 are errors.

In the output `_BitInt(N)` is lowered to the smallest standard integer type
that holds `N` bits, which is also what `sizeof` and `alignof` report:

```c
short a;
unsigned short b;
long c;
unsigned int d;
```

Since the lowered type is wider than `N` bits, the generated code wraps values
to `N` bits itself, so runtime results match the C23 semantics: arithmetic
that produces a `_BitInt(N)` value (`+ - * << ~` and unary `-`) and every
conversion into a `_BitInt(N)` (assignment, initialization, cast, argument
passing, `return`, compound assignment, `++`/`--`) is masked - and sign
extended for the signed types:

```c
unsigned _BitInt(12) a = x;   /* a = ((unsigned short)((unsigned long long)(x) & 0xfffULL)); */
_BitInt(12) b = x;            /* b = ((short)(((long long)((unsigned long long)(x) & 0xfffULL) ^ 0x800LL) - 0x800LL)); */
```

Widths that match the lowered type exactly (`_BitInt(32)`, `_BitInt(64)`) need
no mask, and a value that already has the target `_BitInt` type is not masked
again.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2763.pdf

### 9.22 Decimal Floating Types

**Not implemented.** (`_Decimal32`, `_Decimal64`, `_Decimal128`)

### 9.23 Improved Tag Compatibility

Two `struct`, `union` or `enum` definitions with the same tag and the same
content are the same type, wherever they appear in the translation unit. For a
`struct` or `union` this means the same members, in the same order, with the
same names, types, bit-field widths and alignment; for an `enum` it means the
same underlying type (if one is specified) and the same enumerators, in the
same order, with the same names and values. Before C23 a second definition in
the same scope was an error, and a definition in an inner scope was always a
distinct type.

This makes it possible to define a type at each place it is used, for example
from a macro, instead of requiring one `typedef` per instance:

<!-- runnable -->

```c
#define VEC(T) struct vec_##T { T* data; int size; int capacity; }

/* struct vec_int is defined here, inside the parameter list (prototype scope) */
void push_int(VEC(int)* v, int x)
{
    v->data[v->size++] = x;
}

int main()
{
    int storage[4];
    VEC(int) v = { storage, 0, 4 };   /* same type as push_int's parameter */
    push_int(&v, 1);
}
```

This also applies to a struct defined inside a parameter list: the tag is not
visible after the prototype, but a later definition with the same content is
the same type (gcc 15 accepts this in C23 mode as well).

A redefinition in the same scope with different content is an error (2070),
and definitions with different content in different scopes are distinct,
incompatible types (950 for structs and unions, 40 for enums).

```c
enum E : short { A = 1, B = 2 };
void f(enum E);

void g(void)
{
    enum E : short { A = 1, B = 2 };
    f(A);                             /* same type */
}

void h(void)
{
    enum E : short { A = 1, B = 3 };  /* different value: a new enum E */
    f(A);                             /* warning 40: incompatible types */
}
```

The generated code contains a single definition of the tag: an inner-scope or
repeated definition that has the same content reuses the one already emitted
instead of being renamed.

```c
struct vec_int {
    int * data;
    int size;
    int capacity;
};

void push_int(struct vec_int * v, int x)
{
    v->data[v->size++] = x;
}
```

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3037.pdf

### 9.24 Compound Literals with Storage Specifier

<!-- runnable -->

```c
void F(int* p) {}

int main() {
    F((static int[]){1, 2, 3, 0});
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3038.htm

---

## 10. C2Y Features

These features are from the current C2Y working draft. Support status is noted for each.

### 10.1 Octal Literals with `0o` / `0O` Prefix

<!-- runnable -->

```c
static_assert(0o52 == 052);
static_assert(0O52 == 42);

int main() {
    int i = 0o52;
}
```

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3353.htm

### 10.2 Case Range Expressions

<!-- runnable -->

```c
#include <stdio.h>

void f(int n) {
    switch (n) {
        case 1 ... 10:
            printf("in range 1..10\n");
            break;
        default:
            break;
    }
}
```

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3370.htm

### 10.3 `#def` / `#enddef` Multi-Line Macro Blocks

This feature is implemented in Cake as an experimental extension while its inclusion in C2Y is evaluated.

<!-- runnable -->

```c
/* Use -E to observe expansion */
#def foo(x)
    do {
        bar(x);
        baz(x);
    } while (0)
#enddef

foo(1)
foo(2)
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

### 10.4 `_Countof` Operator

Returns the number of elements in the outermost dimension of an array type.

<!-- runnable -->

```c
int main() {
    int a[7][3];
    static_assert(_Countof(a) == 7);
    static_assert(_Countof(int[2][3]) == 2);
}
```



**Cake extension:** `_Countof` is additionally defined for enum types, returning the number of enumerators. This is not part of C2Y.

<!-- runnable -->

```c
enum E { A, B, C, D, E, F };
static_assert(_Countof(enum E) == 6);
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3369.pdf

### 10.5 `_Defer` Statement

`_Defer` executes a statement or block when the enclosing scope exits, in reverse order of appearance. It is designed for deterministic resource cleanup.

**Key rules:**

- `_Defer` blocks cannot be jumped over with `goto` (constraint violation).
- `_Defer` blocks may not use `break`, `continue`, `return`, or `goto` to exit themselves.
- Execution order is **reverse** — the last `_Defer` encountered in a scope runs first.
- `_Defer` statements that are never reached are never executed.

<!-- runnable -->

```c
#include <stdlib.h>

int main() {
    void* buf = malloc(20);
    _Defer { free(buf); }
    return use_buffer(20, buf);
    /* buf is freed AFTER use_buffer returns */
}
```


**Reverse execution order:**

<!-- runnable -->

```c
int main() {
    int r = 0;
    {
        _Defer {
            _Defer r *= 4;
            r *= 2;
            _Defer { r += 3; }
        }
        _Defer r += 1;
    }
    return r;   /* returns 20 */
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3734.pdf

### 10.6 `if` and `switch` Declarations

The controlling clause of `if` and `switch` (the *selection-header*) accepts three forms:

```
selection-header:
    expression
    declaration expression
    declaration-condition

declaration-condition:
    attribute-specifier-sequence(opt) declaration-specifiers declarator = initializer
```

The third form `if (T D = X)` is treated exactly as `if (T D = X; D)`: the declared
object is the controlling expression. The declared identifier is in scope in the
secondary block and in the `else` branch, and goes out of scope after the statement.

<!-- runnable -->

```c
#include <stdio.h>

int main()
{
   
   FILE* f0;
   if ( f0 = fopen("file.txt", "r"))
   {
     /*...*/
     fclose(f0);
   }
   
   if (FILE* f = fopen("file.txt", "r"); f)
   {
     /*...*/
     fclose(f);
   }

   if (FILE* f = fopen("file.txt", "r"))
   {    
     /*...*/
     fclose(f);
   }

   switch (int c = getchar())
   {
     case EOF: break;
     default: putchar(c); break;
   }
}
```

Constraints (diagnosed as errors):

- A *declaration-condition* declares exactly one object and must have an initializer:
  `if (int x)` and `if (int x = 1, y = 2)` are errors.
- Storage-class specifiers other than `auto`, `constexpr` or `register` are not allowed
  in a *declaration-condition*: `if (static int once = 1)` is an error.
- Since the declared object is the controlling expression, it must have scalar type:
  `if (struct S s = {0})` is an error.
- The clause after the init-statement is an expression, not a declaration:
  `if (int x = 0; int y = x + 1)` (allowed in C++) is an error.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3580.htm

### 10.7 `typename` in `_Generic`

Allows type names as the controlling expression in `_Generic`.

<!-- runnable -->

```c
int main() {
    const int* const p;
    static_assert(_Generic(p, const int*: 1));

    static_assert(_Generic(typeof(p), const int* const: 1));
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3260.pdf

### 10.8 `__COUNTER__` Predefined Macro

<!-- runnable -->

```c
/* Use -E to observe expansion */
#define X(Z) Z Z
X(__COUNTER__)   /* 0 0 */
X(__COUNTER__)   /* 1 1 */
```



Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm

### 10.9 Local Functions

<!-- runnable -->

```c
int main() {
    static int dup(int a) { return a * 2; }
    return dup(1);
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3678.pdf

### 10.10 Function Literals

<!-- runnable -->

```c
#include <stdio.h>

int main() {
    printf("%d", (static int (void)){ return 1; }());
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3679.pdf

### 10.11 Statement Expressions

<!-- runnable -->

```c
#include <stdio.h>

#define maxint(a, b) \
    ({ int _a = (a), _b = (b); _a > _b ? _a : _b; })

int main() {
    printf("%d", maxint(1, 2));
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3643.htm

### 10.12 Elvis Operator (`?:`)

The Elvis operator is a shorthand for the ternary operator where the middle 
operand is omitted. When the condition is truthy, it is returned; 
otherwise the right-hand operand is returned. The condition is evaluated exactly once.

```
a ?: b
```

is equivalent to `a ? a : b`, but `a` is evaluated only once.

<!-- runnable -->

```c
#include <stdio.h>

int main() {
    int x = 0, y = 5;
    int r1 = x ?: y;   /* 5 - x is falsy */
    x = 3;
    int r2 = x ?: y;   /* 3 - x is truthy */
    printf("%d %d\n", r1, r2);
}
```


**Pointer fallback — most common usage:**

```c
const char* display = get_name() ?: "unknown";
```

**Side effects:** when the condition has side effects, Cake introduces a temporary variable in the C89 output to guarantee single evaluation:

```c
int r = i++ ?: b;
```

Generated C89 output:

```c
int __v0;
__v0 = i++;
int r = __v0 ? __v0 : b;
```

**Nesting** associates right-to-left:

```c
int a = 0, b = 0, c = 7;
int r = a ?: b ?: c;   /* 7 */
```

Reference: https://www.open-std.org/JTC1/SC22/WG14/www/docs/n3804.txt

**In preprocessor expressions:** the Elvis operator is also accepted in `#if`/`#elif` constant expressions:

```c
#define VALUE 0

#if VALUE ?: 42
/* VALUE is falsy, so this branch is taken with value 42 */
#endif
```

### 10.13 `static_assert` as an Expression

In C2Y, `static_assert` is extended to work not only as a declaration 
but also as an **expression operator** with type `void`. 
This makes it possible to embed compile-time assertions directly inside 
expressions, which is especially useful when writing function-like macros.

Previously, adding a static check inside a macro required awkward 
workarounds such as negative-size arrays, struct-wrapped assertions, 
or GNU compound statement expressions. 
With this change, `static_assert` can be used naturally in combination with the comma operator.

**Example — bounds-checked bit-shift macro:**

<!-- runnable -->

```c
#include <limits.h>

#define BIT(n) ( \
    static_assert(n >= 0),      \
    static_assert(n < UINT_WIDTH), \
    1U << (n) \
)

int main() {
    unsigned x = BIT(3);   /* ok */
    unsigned y = BIT(99);  /* compile-time error: assertion failed */
}
```


**Disambiguation rule:** A block item consisting solely of `static_assert(...)` followed
by a semicolon is always treated as a `static_assert` *declaration*, preserving 
backward compatibility. `static_assert` is only treated as an *expression* when it 
appears in an expression context (e.g., as an operand of the comma operator, 
or as the controlling expression of `_Generic`).

<!-- runnable -->

```c
void func() {
    static_assert(1);       /* declaration - behaviour unchanged */
    static_assert(1), 0;    /* expression - new in C2Y */
}
```



**Using `_Generic` to produce an integer constant expression:**

Because the comma operator and `void` are not permitted in integer 
constant expressions, the result of the comma-based form cannot be 
used where an integer constant expression is required (e.g., as an array size). 
A `_Generic` workaround can be used in those cases:

<!-- runnable -->

```c

#define CHAR_BIT 8

#define BIT(type, n) \
    _Generic(static_assert(n >= 0 && n < sizeof(type) * CHAR_BIT), \
             void: (type)1 << (n))

int arr[BIT(unsigned int, 2)];   /* ok - integer constant expression */
```


Reference: https://open-std.org/jtc1/sc22/wg14/www/docs/n3715.pdf


Cake extensions `static_debug`, `assert_state`, `override_state` also works in the same way.

<!-- runnable -->

```c
#pragma safety enable
int * _Opt f();
void func() {
    
    int* _Opt p = f();
    p = p ? (static_debug(p), p) : 0;
}
```



## 11. Cake Language Extensions

### 11.1 Built-in `_Assert`

In Cake, `assert` is a built-in statement rather than a macro because flow analysis need it even in release builds.
The effect of `assert(expression)` is equivalent of `if (!(expression)) exit(1);`.

```c
void list_push_back(struct list* list, struct item* _Owner p_item)
{
    if (list->head == NULL) {
        list->head = p_item;
    } else {
        _Assert(list->tail != nullptr);       /* narrows: tail is non-null below */
        _Assert(list->tail->next == nullptr); /* narrows: tail->next is null below */
        list->tail->next = p_item;
    }
    list->tail = p_item;
}
```


### 11.2 `try` / `throw` / `catch`

Cake provides a structured local-jump mechanism for error handling. `try`/`catch` is explicitly a **local** 
jump - it cannot propagate across function boundaries. This is by design.


<!-- runnable -->

```c
extern int error;

int main() {
    try {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (error) throw;
            }
        }
    }
    catch {
        /* handle error */
    }
}
```

> **Note** The `catch` block is optional. `throw` transfers control to the end of the nearest enclosing `try` block.

<button onclick="Try(this)">try</button>

Cake source code uses macros to emulate this feature.

```c
/* emulation of cake try catch using macros */
#define try  
#define catch if (0) catch_label:
#define throw do { throw_break_point(); goto catch_label;}while (0)

```

### 11.3 Checked Expressions (`!` operator)


```
checked-expression:
    assignment-expression
    assignment-expression !
```

The `!` postfix operator evaluates its operand. 
If the result compares equal to zero (or is a null pointer), control transfers to 
the nearest enclosing `catch` block. 
Otherwise the value is returned unchanged. Applicable to any scalar expression.

<!-- runnable -->

```c
int f(void);
int* get_ptr(void);

int main() {
    try {
        int  i = f()!;
        int* p = get_ptr()!;
    }
    catch {
        /* one or more expressions evaluated to zero/null */
    }
}
```


Combined with ownership:

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main() {
    try {
        void* _Owner p = malloc(1)!;
        free(p);
    }
    catch {
        /* malloc returned null */
    }
}
```



> **Note** this is a very experimental feature

### 11.4 `#pragma dir`

Adds a path to the list of directories searched for include files. 
This pragma declares an include directory from inside a source file.
System include directories come from `cake.json` instead (see 2.2), but
`#pragma dir` remains available for project-specific paths.

```c
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt"
#pragma dir "./openssl/include"
```

### 11.5 `#pragma pack`

By default the compiler inserts padding bytes between struct members so that
each member starts at an address that is a multiple of its natural alignment
(4 for `int`, 8 for `double`, and so on), and pads the end of the struct so
that arrays of it keep every element aligned. `#pragma pack(n)` limits that
alignment to at most `n` bytes for every member of the structs declared while
it is in effect. With `pack(1)` no padding is inserted at all, so the struct
occupies exactly the sum of its members' sizes.

It is used to match a layout that is fixed by something other than the
compiler: a file format, a network protocol, a hardware register block, or a
struct shared with code compiled under different settings. The macOS SDK, for
instance, wraps the Mach message structures in `#pragma pack(push, 4)`.
Accessing a misaligned member costs extra instructions on some CPUs and
faults on others, so packing is for data interchange, not a general-purpose
space saver.

The pragma is not part of the C standard, but GCC, Clang and MSVC all
implement it with the same syntax, and Cake follows them: the pack value is
honored when computing `sizeof`, `_Alignof` and `offsetof` of structs and
unions, and a struct declared while a pack value is in effect keeps that
value for its whole lifetime.

```c
#pragma pack(push, n)  // save the current value, then set it to n
#pragma pack(push)     // save the current value
#pragma pack(pop)      // restore the last saved value
#pragma pack(n)        // set it to n
#pragma pack()         // reset to the default (no packing)
```

<!-- runnable -->

```c
#pragma pack(push, 1)
struct packed { char c; int i; };
#pragma pack(pop)

struct normal { char c; int i; };

static_assert(sizeof(struct packed) == 5);
static_assert(offsetof(struct packed, i) == 1);
static_assert(_Alignof(struct packed) == 1);
static_assert(sizeof(struct normal) == 8);
```

In the generated code every struct declared under a pack value is wrapped in
its own `#pragma pack(push, n)` / `#pragma pack(pop)`, so the host compiler
lays it out exactly as Cake did. The wrapper travels with the struct rather
than with the original directive, because Cake emits only the structs a file
uses, in dependency order rather than source order. Given

```c
#pragma pack(push, 1)
typedef struct { char c; int i; } packed_t;
struct named { char c; int i; };
#pragma pack(pop)
struct outer { char c; struct named n; packed_t p; };

int main(void) { struct outer o = {0}; return o.c; }
```

Cake generates

```c
#pragma pack(push, 1)
struct named {
    char c;
    int i;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct __tag2 {
    char c;
    int i;
};
#pragma pack(pop)

struct outer {
    char c;
    struct named n;
    struct __tag2 p;
};

int main(void)
{
    struct outer o;
    __cmemset(&o, 0, 11);
    return o.c;
}
```

Reading the output:

- `struct named` and `struct __tag2` each get their own `push, 1` / `pop`
  because both were declared while `pack(1)` was in effect. `__tag2` is the
  name Cake gives the anonymous struct behind `packed_t`; the typedef itself
  is gone from the output, replaced by the struct tag wherever it was used.
- They are printed before `struct outer` because `outer` contains them by
  value - Cake orders definitions by dependency, not by their position in
  the source, which is why the pragma is attached to each struct instead of
  being copied through from the original file.
- `struct outer` was declared after `#pragma pack(pop)`, so it gets no
  wrapper. Its two packed members still occupy 5 bytes each inside it
  (alignment 1), so `sizeof(struct outer)` is 1 + 5 + 5 = 11.
- `struct outer o = {0}` becomes a declaration plus `__cmemset(&o, 0, 11)`:
  Cake folded `sizeof(struct outer)` to 11 using the packed layout. The host
  compiler, seeing the same pragmas, allocates `o` as 11 bytes too - without
  the wrappers it would allocate 16 and the two sides would disagree.

Bit-fields follow the packed layout of the target compiler too. For GCC and
Clang a bit-field goes at the next free bit and, while a pack value is in
effect, may straddle a storage unit of its type; a zero-width bit-field still
aligns to the natural alignment of its type. MSVC keeps its storage-unit
model with the unit's alignment capped at `n`.

### 11.6 `__attribute__((packed))` and `__attribute__((aligned(n)))`

The GCC/Clang attributes that change struct layout are honored the same way
`#pragma pack` is: they take part in `sizeof`, `_Alignof` and `offsetof`,
and they are carried into the generated code so the host compiler lays the
struct out identically.

- `packed` on a struct or union caps every member at alignment 1 - the same
  layout as `#pragma pack(1)`. All three spellings are accepted:
  `struct __attribute__((packed)) X {...};`, `struct X {...} __attribute__((packed));`
  and `typedef struct {...} __attribute__((packed)) X;`.
- `packed` on a single member gives that member alignment 1 and leaves the
  others alone: `int i __attribute__((packed));` (also before the type, or
  between the type and the name).
- `aligned(n)` on a struct raises its alignment to at least `n` and pads its
  size to a multiple of `n`. `aligned` with no argument means 16.
- `aligned(n)` on a member behaves as `_Alignas(n)`. Like `_Alignas`, it
  only ever raises the alignment; `aligned(2)` on an `int` stays 4.

<!-- runnable -->

```c
struct __attribute__((packed)) header { char tag; int length; };
struct record { char c; int i __attribute__((packed)); short s; };
struct __attribute__((aligned(16))) block { char c; };

static_assert(sizeof(struct header) == 5);
static_assert(_Alignof(struct header) == 1);
static_assert(offsetof(struct record, s) == 6);
static_assert(sizeof(struct record) == 8);
static_assert(sizeof(struct block) == 16);
```

In the output a `packed` struct is wrapped in `#pragma pack(push, 1)` /
`#pragma pack(pop)`, which every host compiler understands, including MSVC;
`aligned(n)` on a struct is written as `__attribute__((aligned(n)))`, or
`__declspec(align(n))` for the MSVC targets; `packed` on a member is written
back as `__attribute__((packed))`. MSVC has no per-member equivalent, so for
the MSVC targets Cake omits it and warns (warning 9) that the generated
struct will not have the packed layout - use `#pragma pack` or pack the whole
struct instead. The example above generates

```c
#pragma pack(push, 1)
struct header {
    char tag;
    int length;
};
#pragma pack(pop)

struct record {
    char c;
    int i __attribute__((packed));
    short s;
};

struct __attribute__((aligned(16))) block {
    char c;
};
```

### 11.7 `offsetof` Operator

In Cake, `offsetof` is a built-in operator rather than a macro, following
[N3958](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3958.pdf)
"Making offsetof a core language feature" (and GCC's `__builtin_offsetof`).
This allows its use in constant expressions and avoids the undefined
behavior associated with traditional macro implementations.

```
offsetof ( type-name , member-designator )

member-designator:
    identifier designator-list_opt

designator:
    [ expression ]
    . identifier
```

* The type-name is real syntax, so a comma inside it (`struct { int x, y; }`) is fine.
* The member-designator may be a path such as `a.b[2].c`, including members of
  anonymous structs/unions.
* An array index that is an integer constant expression keeps the result an
  integer constant expression; an index past the end of a known-size array is
  diagnosed (`-W42`). A flexible array member accepts any index.
* An array index that is *not* constant is allowed; the result is then not a
  constant expression and is emitted as plain arithmetic
  (`constant + (index) * element_size`), never as `__builtin_offsetof`.
* Designating a bit-field, a member that does not exist, subscripting a
  non-array or selecting a member of a non-struct is a constraint violation.

**`offsetof` vs `__builtin_offsetof`**

Cake recognizes two spellings of the operator, and they are the same thing:

| Spelling | Why it exists |
|---|---|
| `offsetof` | The N3958 keyword. Works with no header included. |
| `__builtin_offsetof` | GCC/Clang compatibility. |

Both map to one token in the parser, so they share the same grammar,
evaluation and diagnostics; everything described above applies to either.

This matters because of how system headers define the macro:

```c
/* GCC / Clang <stddef.h> */
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

/* MSVC <stddef.h> */
#define offsetof(s, m) ((size_t)&(((s*)0)->m))
```

With `<stddef.h>` included, `offsetof(T, m)` is first expanded by the
preprocessor and then parsed by Cake:

* GCC/Clang headers expand it to `__builtin_offsetof(...)`, which Cake parses
  as the operator, so the result is an integer constant expression exactly as
  if the keyword had been used directly.
* MSVC headers expand it to a null-pointer cast. Cake handles this by treating
  `&(((T*)0)->m)` as a special case that folds to a constant (see the macro
  note below), so the result is still usable in `static_assert` and other
  constant contexts.

Without `<stddef.h>`, `offsetof` is simply the keyword. In every case the
generated C89 code contains the computed byte offset (or, for a non-constant
index, plain arithmetic), never a call to `__builtin_offsetof`, so the output
does not depend on the target compiler providing a builtin.

Because `offsetof` is a keyword in every `-std` mode (C89 through C2Y), a
program that declares its own identifier named `offsetof` is rejected. This
is stricter than N3958, which reserves the name only in the new standard, but
the name has been reserved by `<stddef.h>` since C89 so conforming code is
never affected.

```c
struct foo { int N; struct { int a; int b[4]; } in; char buf[]; };

static_assert(offsetof(struct foo, in.b[2]) == 16);

size_t buf_offsetof(int i)
{
    return offsetof(struct foo, buf[i]); /* not a constant expression */
}
```

<!-- runnable -->

```c
#include <stdio.h>

struct S {
    char   m0;
    double m1;
    short  m2;
    char   m3;
};

int main() {
    printf("m0 offset = %zu\n", offsetof(struct S, m0));
    printf("m1 offset = %zu\n", offsetof(struct S, m1));
    printf("m2 offset = %zu\n", offsetof(struct S, m2));
    printf("m3 offset = %zu\n", offsetof(struct S, m3));
}
```


Cake also supports the traditional macro-based `offsetof` (as found in MSVC's
`<stddef.h>` or in user code) by making an exception in the constant-expression
rules: `&(((T*)0)->member)` is folded to the member offset at compile time
instead of being rejected as a non-constant address.

```c
#define offsetof(type, member) ((size_t)&(((type *)0)->member))
```


### 11.8 Type Traits

Cake provides compile-time type introspection functions that return boolean integer constant expressions:


**`_is_pointer(T)`** — true for pointers to objects or functions

**`_is_array(T)`** — true for array types

**`_is_function(T)`** — true for function types

<!-- runnable -->

```c
int main()
{
  static_assert(_is_function(main));
  int a[10];
  static_assert(_is_array(a));
  int (*b)[2];
  static_assert(_is_pointer(b));
}
```


### 11.9 Object Lifetime Checks (Ownership)

Cake includes a static analysis system for tracking object ownership and lifetime, activated with `#pragma safety enable` or `-fanalyzer`.

**`_Owner`** — This pointer or variable is responsible for freeing the object.

**`_Opt`** — This pointer may be null (optional).

**`_View`** — This pointer borrows the object without owning it.

To annotate the standard library, Cake ships its own annotated headers ahead
of the system ones on the include search path and chains to the real header
with `#include_next` (2.3).

Full documentation: [ownership.html](ownership.html)

### 11.10 Pre-defined Macros

Cake defines the following macros:

```c
#define __CAKE__          202311L
#define __STDC_VERSION__  202311L
```

---

## 12. GCC Compatibility

Cake recognizes the following GCC built-ins and extensions:

- `__builtin_va_list`
- `__builtin_va_start`
- `__builtin_c23_va_start`
- `__builtin_va_end`
- `__builtin_va_arg`
- `__builtin_va_copy`
- `__builtin_offsetof` — same as Cake's built-in `offsetof`
- `__attribute__` — `packed` and `aligned(n)` affect layout (see 11.6); the others are accepted and ignored
- `__typeof__` — alias for `typeof`

Additional GCC built-ins are declared in `src/include/x86_x64_gcc_builtins.h`.

Pre-defined GCC compatibility macros: https://gcc.gnu.org/onlinedocs/cpp/Predefined-Macros.html

---

## 13. MSVC Compatibility

Cake recognizes the following MSVC extensions:

- `__ptr32`, `__ptr64`
- `__int8` through `__int64` — fixed-width integer types
- `__declspec`
- `__cdecl`
- `__fastcall`, `__stdcall` — calling conventions
- `__forceinline` — alias for `inline` in Cake
- `__pragma`
- `__unaligned`

Pre-defined MSVC compatibility macros: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros

See also: `src/include/x86_msvc_macros.h`, `src/include/x64_msvc_macros.h`

---

## Appendix A. Grammar of the Generated Code

This appendix defines the subset of C that the Cake backend emits (section 5). It is intended for anyone who 
wants to write a new backend, or a tool that consumes Cake output: a consumer that handles exactly this 
grammar handles every file Cake can generate. It is deliberately much smaller than the C89 grammar — 
there are no typedefs, enums, `switch`, `sizeof`, `const`, character constants, nested types, initializers 
on local objects, or declarations with more than one declarator.

Notation: `A ::= B | C` defines `A`; `X?` means optional; `X*` means zero or more; `X+` means one or more; 
terminals are quoted. Names in *italics* are lexical (A.6). Rules marked **[gcc]** or **[msvc]** are 
emitted only for targets of that family.

### A.1 Translation unit

```
translation-unit ::=
      header-comment external-declaration*

header-comment ::=
      "/* Cake " version " " target-name " " date " " time " */"

external-declaration ::=
      struct-or-union-definition
    | pragma-pack-block
    | object-declaration
    | function-declaration
    | function-definition
```

Order inside the file is: all `struct-or-union-definition`s (in dependency order), then helper 
`function-declaration`s, then hoisted statics, then the remaining declarations and definitions in source 
order, then helper `function-definition`s (5.3).

### A.2 Types

```
struct-or-union-definition ::=
      alignment-prefix? struct-or-union identifier '{' member-declaration+ '}' ';'

pragma-pack-block ::=
      "#pragma pack(push, " integer-constant ")" newline
      struct-or-union-definition
      "#pragma pack(pop)" newline

alignment-prefix ::=
      "struct __attribute__((aligned(" integer-constant ")))"      [gcc]  — replaces the "struct" keyword
    | "__declspec(align(" integer-constant "))"                    [msvc] — precedes the "struct" keyword

struct-or-union ::=
      "struct" | "union"

member-declaration ::=
      type-specifier declarator member-suffix? ';'

member-suffix ::=
      ':' integer-constant                       — bit-field
    | "__attribute__((packed))"                  [gcc]

type-specifier ::=
      "volatile"? base-type

base-type ::=
      "void"
    | "char" | "signed char" | "unsigned char"
    | "short" | "unsigned short"
    | "int" | "unsigned int"
    | "long" | "unsigned long"
    | "long long" | "unsigned long long"
    | "float" | "double" | "long double"
    | struct-or-union identifier
```

There is no `typedef`, `enum`, `const`, `restrict`, `_Bool`, `_Complex`, `_BitInt` or `_Atomic` in the 
output. `bool` has already become `unsigned char`; enumerations have become `int` and their constants 
integer literals. Every `struct`/`union` is referred to by tag; there are no anonymous or nested definitions.

### A.3 Declarators

```
declarator ::=
      pointer? direct-declarator

pointer ::=
      '*'+                                         — qualifiers on pointers are dropped

direct-declarator ::=
      identifier
    | '(' calling-convention? '*' identifier ')' parameter-clause      — pointer to function
    | direct-declarator '[' integer-constant ']'                       — array (size always constant)
    | calling-convention? identifier parameter-clause                  — function

calling-convention ::=
      "__cdecl" | "__stdcall" | "__fastcall"                           [msvc]

parameter-clause ::=
      '(' "void" ')'
    | '(' parameter-declaration (',' parameter-declaration)* (',' "...")? ')'

parameter-declaration ::=
      type-specifier abstract-or-named-declarator

abstract-or-named-declarator ::=
      pointer? identifier?                                              — identifier omitted in prototypes
    | pointer? identifier? '[' integer-constant ']'
    | pointer? '(' '*' identifier? ')' parameter-clause
```

Every function declarator has a prototype. Array parameters and VLA parameters have already been 
rewritten as pointers. Array dimensions are always integer literals: variably-modified types have been 
lowered to pointers plus `alloca` (5.4.8).

### A.4 Declarations and definitions

```
object-declaration ::=
      storage-class? type-specifier declarator ('=' initializer)? ';'

storage-class ::=
      "static" | "extern"

initializer ::=
      constant-expression
    | string-literal
    | identifier                                  — address of a function (e.g. "= printf")
    | '{' initializer-item (',' initializer-item)* '}'

initializer-item ::=
      constant-expression
    | string-literal
    | identifier

function-declaration ::=
      storage-class? type-specifier declarator ';'

function-definition ::=
      storage-class? type-specifier declarator compound-statement
```

Each declaration declares exactly one identifier. Initializers appear only on file-scope objects and are 
always flat: designators and nested braces have been removed (5.4.3). Local objects are never initialized 
in their declaration, with one exception: the temporary that holds a `switch` controlling expression 
(`int __a = n;`, 5.4.6).

### A.5 Statements

```
compound-statement ::=
      '{' local-declaration* statement* '}'

local-declaration ::=
      type-specifier declarator ('=' expression)? ';'   — initializer only on the switch temporary "__a" (5.4.6)

statement ::=
      compound-statement
    | expression-statement
    | if-statement
    | while-statement
    | do-statement
    | for-statement
    | labeled-statement
    | jump-statement
    | ';'

expression-statement ::=
      expression ';'

if-statement ::=
      "if" '(' expression ')' statement
    | "if" '(' expression ')' statement "else" statement
    | "if" '(' expression ')' statement "else" identifier ':' statement     — catch label (5.4.9)

while-statement ::=
      "while" '(' expression ')' statement

do-statement ::=
      "do" statement "while" '(' expression ')' ';'

for-statement ::=
      "for" '(' expression? ';' expression? ';' expression? ')' statement

labeled-statement ::=
      identifier ':' statement

jump-statement ::=
      "goto" identifier ';'
    | "continue" ';'
    | "break" ';'                                   — only inside loops
    | "return" expression? ';'
```

All local declarations precede all statements in a block (C89 rule). There is no `switch`, `case` or 
`default`; `break` never targets a `switch`. There are no declarations in the `for` clause. 
Comments may appear anywhere and carry no meaning.

### A.6 Expressions

The expression grammar is the C89 expression grammar without `sizeof`. All C operators and precedence 
levels are used.

```
expression ::=
      assignment-expression (',' assignment-expression)*

assignment-expression ::=
      conditional-expression
    | unary-expression assignment-operator assignment-expression

assignment-operator ::=
      '=' | "*=" | "/=" | "%=" | "+=" | "-=" | "<<=" | ">>=" | "&=" | "^=" | "|="

conditional-expression ::=
      logical-or-expression ('?' expression ':' conditional-expression)?

logical-or-expression ::=
      logical-and-expression ("||" logical-and-expression)*

logical-and-expression ::=
      inclusive-or-expression ("&&" inclusive-or-expression)*

inclusive-or-expression ::=
      exclusive-or-expression ('|' exclusive-or-expression)*

exclusive-or-expression ::=
      and-expression ('^' and-expression)*

and-expression ::=
      equality-expression ('&' equality-expression)*

equality-expression ::=
      relational-expression (("==" | "!=") relational-expression)*

relational-expression ::=
      shift-expression (('<' | '>' | "<=" | ">=") shift-expression)*

shift-expression ::=
      additive-expression (("<<" | ">>") additive-expression)*

additive-expression ::=
      multiplicative-expression (('+' | '-') multiplicative-expression)*

multiplicative-expression ::=
      cast-expression (('*' | '/' | '%') cast-expression)*

cast-expression ::=
      unary-expression
    | '(' type-name ')' cast-expression

unary-expression ::=
      postfix-expression
    | "++" unary-expression
    | "--" unary-expression
    | ('&' | '*' | '+' | '-' | '~' | '!') cast-expression

postfix-expression ::=
      primary-expression
    | postfix-expression '[' expression ']'
    | postfix-expression '(' (assignment-expression (',' assignment-expression)*)? ')'
    | postfix-expression '.' identifier
    | postfix-expression "->" identifier
    | postfix-expression "++"
    | postfix-expression "--"

primary-expression ::=
      identifier
    | integer-constant
    | floating-constant
    | string-literal
    | '(' expression ')'

type-name ::=
      type-specifier abstract-declarator?

abstract-declarator ::=
      pointer
    | pointer? '(' '*' ')' parameter-clause
    | pointer? '[' integer-constant ']'

constant-expression ::=
      conditional-expression                       — composed only of literals and unary/binary operators
```

Absent from the output: `sizeof`, `_Alignof`, `_Countof`, `_Generic`, `typeof`, compound literals, 
statement expressions, the Elvis operator, character constants, enumeration constants, `nullptr`, 
`true`/`false`. The only identifiers that appear in expressions are objects, functions, members, labels and 
the generated names of section 5.5. Calls to `__builtin_alloca` / `_alloca` (5.4.8) and to the helpers of 
section 5.6 are ordinary function calls.

### A.7 Lexical elements

```
identifier ::=
      nondigit (nondigit | digit)*                 — includes generated names: "__c…", "__L…", "__a", "__b",
                                                     "__vm…", "__tag…", "__m…", "__cake_func_…"

integer-constant ::=
      '-'? decimal-digits integer-suffix?

integer-suffix ::=
      "U" | "L" | "UL" | "LL" | "ULL"

floating-constant ::=
      '-'? decimal-digits '.' decimal-digits exponent? floating-suffix?
    | '-'? decimal-digits exponent floating-suffix?

exponent ::=
      ('e' | 'E') ('+' | '-')? decimal-digits

floating-suffix ::=
      'f' | 'L'

string-literal ::=
      'L'? '"' s-char* '"'                         — C89 escape sequences; no u8/u/U prefixes

comment ::=
      "/*" any-text "*/"
```

Integer constants are always decimal (hexadecimal, octal and binary literals are folded). Negative values 
appear as a `-` sign followed by the magnitude. Adjacent string literals are concatenated by Cake, so a 
string literal is always a single token.

### A.8 Target-dependent spellings

The grammar above is the same for every target; only a few terminals change:

| Item | GCC / Clang / Catalina targets | MSVC targets |
|---|---|---|
| `alloca` | `__builtin_alloca` | `_alloca` |
| aligned type | `struct __attribute__((aligned(n))) T` | `__declspec(align(n)) struct T` |
| per-member packed | `__attribute__((packed))` | not emitted |
| calling conventions | not emitted | `__cdecl`, `__stdcall`, `__fastcall` |
| `size_t` in helper prototypes and `sizeof` suffix | `unsigned long` / `UL` (LP64) | `unsigned long long` / `ULL` (x64), `unsigned int` / `U` (x86) |

`#pragma pack(push, n)` / `#pragma pack(pop)` is emitted identically for all targets.

---

*End of Cake C Compiler Reference Manual*
