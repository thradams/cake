# Cake C Compiler — Reference Manual

## 1. Introduction

Cake is a source-to-source C compiler that translates modern C (C99 through C2Y) into C89-compatible output, suitable for compilation by existing toolchains such as GCC and MSVC.

Cake integrates directly into existing build environments:

- On **Windows**, it operates as an extension for MSVC.
- On **Linux**, it operates as an extension for GCC.

Where applicable, Cake uses the same command-line flags as GCC and MSVC to minimize friction when integrating into existing projects.

---

## 2. Getting Started

### 2.1 Platform Support

Cake runs on Windows (targeting MSVC) and Linux (targeting GCC). 
Cake can also perform cross-compilation, as long as you provide the headers from the target platform.

### 2.2 Include Directories

Include directories are specified in a `cakeconf.h` configuration header.
Cake first searches for this file relative to the source file being compiled. 
If no local `cakeconf.h` is found, the root `cakeconf.h` from the installation directory is used.


**Discovering system include paths manually:**

On Windows (from a Visual Studio Developer Command Prompt):

```
echo %INCLUDE%
```

On Linux:

```
echo | gcc -E -Wp,-v -
```

**Sample `cakeconf.h` for Linux:**

```c
#ifdef __linux__
#pragma dir "/usr/lib/gcc/x86_64-linux-gnu/11/include"
#pragma dir "/usr/local/include"
#pragma dir "/usr/include/x86_64-linux-gnu"
#pragma dir "/usr/include"
#endif
```

**Sample `cakeconf.h` for Windows:**

```c
#ifdef _WIN32
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/include"
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/ATLMFC/include"
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/VS/include"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/ucrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/um"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/shared"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/winrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um"
#endif
```

**Per-project configuration:**

Projects can have their own local `cakeconf.h` that includes a shared system-level configuration and adds project-specific paths:

`yourproject\cakeconf.h`:

```c
// System includes
#include "C:\Program Files (x86)\cake\cakeconf.h"

// Project-specific includes
#pragma dir ".\openssl\include"
```

### 2.3 System include paths auto-configuration

The `-auto-config` option generates a root `cakeconf.h` automatically by querying the active compiler environment:

```
cake -auto-config
```

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
Compiles `source.c` and writes the C89 output to `/[default-target]/source.c`.

```
cake -target=X86_msvc source.c
```
Compiles `source.c` targeting MSVC x86 and writes output to `/X86_msvc/source.c`.

```
cake file.c -o file.cc && cl file.cc
```
Compiles `file.c` to `file.cc`, then passes `file.cc` to the MSVC compiler.

---

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
`-Wstyle -msvc-output -no-output -sarif -sarif-path "$(SolutionDir).sarif" $(ItemPath)`

**`-msvc-output`**  
Format diagnostic output to be compatible with the Visual Studio IDE error parser.

**`-fdiagnostics-color=never`**  
Disable ANSI color codes in diagnostic output. Same as GCC.

### 4.3 Diagnostic Options

**`-W<number>`**  
Enable warning number `<number>`. See the [Warnings Reference](warnings.html).

**`-Wno-<number>`**  
Disable warning number `<number>`.

**`-Wall`**  
Enable all warnings.

**`-disable-assert`**  
Disable Cake's built-in `assert` statement extension and revert to standard macro behavior.

### 4.4 Target Options

**`-target=<name>`**  
Set the compilation target. Controls integer sizes, alignment, and the style of generated C89 output.

Available targets:

- `x86_x64_gcc` — Linux x86-64, GCC output
- `x86_msvc` — Windows x86, MSVC output
- `x64_msvc` — Windows x64, MSVC output
- `catalina` — Catalina C compiler
- `ccu8` — Embedded / custom target

**`-auto-config`**  
Generate a `cakeconf.h` header file configured for the current system.

### 4.5 Analysis Options

**`-fanalyzer`**  
Run Cake's built-in flow analysis pass, including ownership, nullability, and lifetime checks.

**`-const-literal`**  
Treat string literals as `const char[]` rather than `char[]`.

### 4.6 Style and Formatting Options

**`-style=<name>`**  
Set the naming-style convention used by style warnings (`-w011`). Available styles: `cake`, `gnu`, `microsoft`.

### 4.7 Using cake inside Visual Studio
Use cake as Custom Build Tool for a specific file.c

- Command line: `cake -sarif -sarif-path "$(SolutionDir).sarif" -line-directives -msvc-output file.c`
- Ouputs: `x86/file.c`
- Add Ouput to item type: `C/C++ Compiler`

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

- No preprocessor directives
- No `typedef` declarations (expanded inline)
- No `enum` declarations (replaced with integer constants)
- No `const` qualifiers
- No constant expressions (evaluated at translation time)
- No `switch` statements (lowered to equivalent constructs)
- No nested `struct`/`union` definitions
- No `sizeof` expressions (evaluated at translation time)
- No local `static` variables (hoisted to file scope)
- Array sizes are always explicit integer constants
- Function prototypes are generated automatically

It currently requires bit-fields, but they are planned for removal.

### 5.2 Output

For a single input file:

```
cake c:\project\file1.c
```

```
c:\project\
├── file1.c
└── target\
    └── file1.c
```
or using `-o` option:

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
then recreates the directory structure under `target` within this common path.


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

```c
void f(const char* restrict s);
int main() { f(""); }
```
<button onclick="Try(this)">try</button>

### 7.2 Variably-Modified (VM) Types and Variable-Length Arrays

**VM type pointer:**

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
<button onclick="Try(this)">try</button>

**VLA as 2D function parameter:**

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
<button onclick="Try(this)">try</button>


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n683.htm

### 7.3 Flexible Array Members

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
<button onclick="Try(this)">try</button>

The size of a structure with a flexible array member is computed as if the member were omitted, except that additional trailing padding may be present.

### 7.4 `static` and Type Qualifiers in Array Declarators

The `static` keyword in array declarators is parsed and checked. Passing `NULL`, `nullptr`, or an array smaller than the declared minimum is a diagnostic.

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
<button onclick="Try(this)">try</button>

### 7.5 Hexadecimal Floating Constants

```c
double d = 0x1p+1;
```

Cake converts hexadecimal floating-point literals to decimal representation using `strtod` followed by `snprintf`. This conversion may introduce minor precision loss.

### 7.6 Compound Literals

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
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n716.htm

### 7.7 Designated Initializers

```c
int main() {
    int a[6] = {[4] = 29, [2] = 15};
    struct point { int x, y; };
    struct point p = { .y = 2, .x = 3 };
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n494.pdf

### 7.8 Declarations in `for` Loop Initializers

```c
int main() {
    const int max = 10;
    for (int n = max - 1; n >= 0; n--) {
        /* ... */
    }
}
```
<button onclick="Try(this)">try</button>

### 7.9 `inline` Functions

```c
inline int sum(int a, int b) { return a + b; }

int main(void) {
    int r = sum(1, 2);
}
```
<button onclick="Try(this)">try</button>

Inline functions in Cake are equivalent to static, since Cake does 
not currently perform function inlining.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n709.htm

### 7.10 `_Pragma` Preprocessing Operator

```c
#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)
LISTING(..listing.dir)
```

### 7.11 `__func__` Predefined Identifier

```c
#include <stdio.h>
int main() {
    printf("%s\n", __func__);
}
```

<button onclick="Try(this)">try</button>

### 7.12 Variadic Macros

```c
#include <stdio.h>
#define debug(...) fprintf(stderr, __VA_ARGS__)

int main() {
    debug("X = %d\n", 1);
}
```

<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n707.htm

### 7.13 `_Bool`

```c
int main(void) {
    _Bool b = 1;
}
```

<button onclick="Try(this)">try</button>

### 7.14 Line Comments

C99 `//` line comments are implemented. 

### 7.15 Complex and Imaginary Types

**Not implemented.**

### 7.16 Universal Character Names (`\u`, `\U`)

**Not implemented.**

---

## 8. C11 Features

### 8.1 `_Static_assert` / `static_assert`

```c
int main() {
    _Static_assert(1 == 1, "error");
}
```
<button onclick="Try(this)">try</button>

`_Static_assert` is aliased to `static_assert` in C23.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1330.pdf

### 8.2 Anonymous Structures and Unions

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
<button onclick="Try(this)">try</button>

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

```c
#include <math.h>

#define cbrt(X) _Generic((X),    \
                  double: cbrtl, \
                  float: cbrtf,  \
                  default: cbrtl \
              )(X)

int main(void) { cbrt(1.0); }
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1441.htm

### 8.6 Unicode Character Constants (`u''`, `U''`)

```c
int i  = U'ç';
int i2 = u'ç';
```
<button onclick="Try(this)">try</button>

> **Note:** Cake assumes source files are UTF-8 encoded.

### 8.7 UTF-8 String Literals (`u8"..."`)

```c
char* s1 = u8"maçã";
char* s2 = u8"maca";
```
<button onclick="Try(this)">try</button>

> **Note:** Cake assumes source files are UTF-8 encoded.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1488.htm

### 8.8 `_Alignof` / `alignof`

```c
int main() {
    int align = alignof(int);
}
```
<button onclick="Try(this)">try</button>

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

```c
/* VM type pointer - mandatory in C23 */
void foo(int n, double (*x)[n]) {
    (*x)[0] = 1.0;
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2778.pdf

### 9.2 `static_assert` — Single-Argument Form

```c
int main(void) {
    static_assert(1 == 2);   /* no message argument required */
}
```
<button onclick="Try(this)">try</button>

### 9.3 `u8` Character Prefix

```c
int main() {
    unsigned char c = u8'~';
}
```
<button onclick="Try(this)">try</button>

Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf

### 9.4 No Function Declarations Without Prototypes

In C23, calling an undeclared function is a constraint violation:

```c
int main() {
    func();   /* error in C23 */
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/JTC1/SC22/WG14/www/docs/n2841.htm

### 9.5 Unnamed Parameters in Function Definitions

```c
int f(int);

int f(int) {   /* unnamed parameter is valid */
    return 0;
}
```
<button onclick="Try(this)">try</button>

> **Note:** TODO add placeholder name in the C89 output.

Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n2480.pdf

### 9.6 Digit Separators

```c
int main() {
    int a = 1000'00;
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2626.pdf

### 9.7 Binary Literals

```c
#define X  0b1010

int main() {
    int a = X;
    int b = 0B1010;
}
```
<button onclick="Try(this)">try</button>

### 9.8 `nullptr` Constant

```c
int main() {
    void* p          = nullptr;
    auto  p2         = nullptr;
    typeof(nullptr) p3 = nullptr;
}
```
<button onclick="Try(this)">try</button>

Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n3042.htm

### 9.9 `true` and `false` as First-Class Keywords

```c
int main() {
    bool b = true;
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2935.pdf

### 9.10 Empty Initializer `{}`

```c
int main() {
    struct X { int i; } x = {};
    x = (struct X){};
}
```
<button onclick="Try(this)">try</button>

> **Note** Empty initializer can be used initialize VLAs

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2900.htm

### 9.11 `auto` Type Deduction

```c
static auto a = 3.5;
auto p = &a;

double A[3] = { 0 };
auto pA = A;
auto qA = &A;
```
<button onclick="Try(this)">try</button>

Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n3007.htm

### 9.12 `typeof` / `typeof_unqual`

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
<button onclick="Try(this)">try</button>

Reference: https://open-std.org/JTC1/SC22/WG14/www/docs/n2927.htm

### 9.13 `constexpr`

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
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3018.htm

### 9.14 Enhancements to Enumerations (Typed Enums)

```c
enum X : short { A };

int main() {
    enum X x = A;
}
```
<button onclick="Try(this)">try</button>

> **Note** TODO Missing some details.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm

### 9.15 Attributes (`[[...]]`)

Cake supports the C23 double-bracket attribute syntax. Recognized standard attributes:

**`[[fallthrough]]`** *(Partial — parsed; enforcement pending)*  
Suppresses the fallthrough diagnostic on a switch case.

**`[[deprecated]]`** *(Supported)*  
Emits a warning when the annotated entity is used. Compile with `-w03`.

```c
[[deprecated]] void f2(void) {}
struct [[deprecated]] S { int a; };

int main(void) {
    struct S s;
    f2();         /* warning: deprecated */
}
```

<button onclick="Try(this)">try</button>


**`[[maybe_unused]]`** *(Supported)*  
Suppresses unused-variable or unused-parameter diagnostics.

```c
void f([[maybe_unused]] int arg1, int arg2) {}
```

**`[[nodiscard]]`** *(Supported — optional message argument not yet implemented)*  
Emits a warning when the return value of the annotated function is discarded.

```c
struct [[nodiscard]] error_info { int error; };
struct error_info enable_safety(void);

void test(void) {
    enable_safety();   /* warning: return value discarded */
}
```

<button onclick="Try(this)">try</button>

**`[[noreturn]]`** *(Supported)*  
Replaces C11 `_Noreturn`.

**`[[unsequenced]]`** / **`[[reproducible]]`** *(TODO)*

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2335.pdf

### 9.16 `__has_attribute` / `__has_include`

```c
#if __has_c_attribute(fallthrough)
#  warning Attribute supported
#endif

#if __has_include(<stdio.h>)
#  warning Header found
#endif
```
<button onclick="Try(this)">try</button>


### 9.17 `#warning` Directive

```c
int main() {
#warning my warning message
}
```

<button onclick="Try(this)">try</button>


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2686.pdf

### 9.18 `#embed` Directive


```c
#include <stdio.h>

int main() {
    static const char file_txt[] = {
#embed "stdio.h"
        , 0
    };
    printf("%s\n", file_txt);
}
```

<button onclick="Try(this)">try</button>

> **Note** Some details are not implemented yet.

### 9.19 `#elifdef` / `#elifndef`

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
<button onclick="Try(this)">try</button>

### 9.20 `__VA_OPT__`

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
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3033.htm

### 9.21 `_BitInt(N)` — Bit-Precise Integers

**Not implemented yet**

```c
int main() {
    _BitInt(2)  a2  = 1;
    _BitInt(33) a33 = 1;
}
```

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2763.pdf

### 9.22 Decimal Floating Types

**Not implemented.** (`_Decimal32`, `_Decimal64`, `_Decimal128`)

### 9.23 Improved Tag Compatibility

**Not implemented yet.**

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3037.pdf

### 9.24 Compound Literals with Storage Specifier

```c
void F(int* p) {}

int main() {
    F((static int[]){1, 2, 3, 0});
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3038.htm

---

## 10. C2Y Features

These features are from the current C2Y working draft. Support status is noted for each.

### 10.1 Octal Literals with `0o` / `0O` Prefix

```c
static_assert(0o52 == 052);
static_assert(0O52 == 42);

int main() {
    int i = 0o52;
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3353.htm

### 10.2 Case Range Expressions

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
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3370.htm

### 10.3 `#def` / `#enddef` Multi-Line Macro Blocks

This feature is implemented in Cake as an experimental extension while its inclusion in C2Y is evaluated.

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
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

### 10.4 `_Countof` Operator

Returns the number of elements in the outermost dimension of an array type.

```c
int main() {
    int a[7][3];
    static_assert(_Countof(a) == 7);
    static_assert(_Countof(int[2][3]) == 2);
}
```

<button onclick="Try(this)">try</button>

**Cake extension:** `_Countof` is additionally defined for enum types, returning the number of enumerators. This is not part of C2Y.

```c
enum E { A, B, C, D, E, F };
static_assert(_Countof(enum E) == 6);
```
<button onclick="Try(this)">try</button>


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3369.pdf

### 10.5 `_Defer` Statement

`_Defer` executes a statement or block when the enclosing scope exits, in reverse order of appearance. It is designed for deterministic resource cleanup.

**Key rules:**

- `_Defer` blocks cannot be jumped over with `goto` (constraint violation).
- `_Defer` blocks may not use `break`, `continue`, `return`, or `goto` to exit themselves.
- Execution order is **reverse** — the last `_Defer` encountered in a scope runs first.
- `_Defer` statements that are never reached are never executed.

```c
#include <stdlib.h>

int main() {
    void* buf = malloc(20);
    _Defer { free(buf); }
    return use_buffer(20, buf);
    /* buf is freed AFTER use_buffer returns */
}
```
<button onclick="Try(this)">try</button>


**Reverse execution order:**

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
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3734.pdf

### 10.6 `if` Declarations

A declaration can appear in the initializer clause of an `if` statement, scoping the declared variable to the entire `if`/`else` chain.

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
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3388.htm

### 10.7 `typename` in `_Generic`

Allows type names as the controlling expression in `_Generic`.

```c
int main() {
    const int* const p;
    static_assert(_Generic(p, const int*: 1));

    static_assert(_Generic(typeof(p), const int* const: 1));
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3260.pdf

### 10.8 `__COUNTER__` Predefined Macro

```c
/* Use -E to observe expansion */
#define X(Z) Z Z
X(__COUNTER__)   /* 0 0 */
X(__COUNTER__)   /* 1 1 */
```

<button onclick="Try(this)">try</button>


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm

### 10.9 Local Functions

```c
int main() {
    static int dup(int a) { return a * 2; }
    return dup(1);
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3678.pdf

### 10.10 Function Literals

```c
#include <stdio.h>

int main() {
    printf("%d", (static int (void)){ return 1; }());
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3679.pdf

### 10.11 Statement Expressions

```c
#include <stdio.h>

#define maxint(a, b) \
    ({ int _a = (a), _b = (b); _a > _b ? _a : _b; })

int main() {
    printf("%d", maxint(1, 2));
}
```
<button onclick="Try(this)">try</button>

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3643.htm

### 10.12 Elvis Operator (`?:`)

The Elvis operator is a shorthand for the ternary operator where the middle 
operand is omitted. When the condition is truthy, it is returned; 
otherwise the right-hand operand is returned. The condition is evaluated exactly once.

```
a ?: b
```

is equivalent to `a ? a : b`, but `a` is evaluated only once.

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

<button onclick="Try(this)">try</button>

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

<button onclick="Try(this)">try</button>

**Disambiguation rule:** A block item consisting solely of `static_assert(...)` followed
by a semicolon is always treated as a `static_assert` *declaration*, preserving 
backward compatibility. `static_assert` is only treated as an *expression* when it 
appears in an expression context (e.g., as an operand of the comma operator, 
or as the controlling expression of `_Generic`).

```c
void func() {
    static_assert(1);       /* declaration - behaviour unchanged */
    static_assert(1), 0;    /* expression - new in C2Y */
}
```

<button onclick="Try(this)">try</button>

**Using `_Generic` to produce an integer constant expression:**

Because the comma operator and `void` are not permitted in integer 
constant expressions, the result of the comma-based form cannot be 
used where an integer constant expression is required (e.g., as an array size). 
A `_Generic` workaround can be used in those cases:

```c

#define CHAR_BIT 8

#define BIT(type, n) \
    _Generic(static_assert(n >= 0 && n < sizeof(type) * CHAR_BIT), \
             void: (type)1 << (n))

int arr[BIT(unsigned int, 2)];   /* ok - integer constant expression */
```

<button onclick="Try(this)">try</button>

Reference: https://open-std.org/jtc1/sc22/wg14/www/docs/n3715.pdf


Cake extensions `static_debug`, `assert_state`, `override_state` also works in the same way.

```c
#pragma safety enable
int * _Opt f();
void func() {
    
    int* _Opt p = f();
    p = p ? (static_debug(p), p) : 0;
}
```
<button onclick="Try(this)">try</button>

---

## 11. Cake Language Extensions

### 11.1 Built-in `assert`

In Cake, `assert` is a built-in statement rather than a macro because flow analysis need it even in release builds.
The effect of `assert(expression)` is equivalent of `if (!(expression)) exit(1);`.

```c
void list_push_back(struct list* list, struct item* _Owner p_item)
{
    if (list->head == NULL) {
        list->head = p_item;
    } else {
        assert(list->tail != nullptr);       /* narrows: tail is non-null below */
        assert(list->tail->next == nullptr); /* narrows: tail->next is null below */
        list->tail->next = p_item;
    }
    list->tail = p_item;
}
```

<button onclick="Try(this)">try</button>


To disable this built-in behavior and use a standard macro instead, pass `-disable-assert`.

### 11.2 `try` / `throw` / `catch`

Cake provides a structured local-jump mechanism for error handling. `try`/`catch` is explicitly a **local** 
jump - it cannot propagate across function boundaries. This is by design.



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

<button onclick="Try(this)">try</button>

Combined with ownership:

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

<button onclick="Try(this)">try</button>

> **Note** this is a very experimental feature

### 11.4 `#pragma dir`

Adds a path to the list of directories searched for include files. 
This is the pragma used in `cakeconf.h` to declare system and project include directories.

```c
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt"
#pragma dir "./openssl/include"
```

### 11.5 `offsetof` Operator

In Cake, `offsetof` is a built-in operator rather than a macro.  Similar of GCC `__builtin_offsetof`.
This allows its use in constant expressions and avoids the undefined 
behavior associated with traditional macro implementations.

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

<button onclick="Try(this)">try</button>


Cake also supports compile time macro based `offsetof` by creating exceptions for constant 
expressions involving addresses.

```c
#define offsetof(type, member) ((size_t)&(((type *)0)->member))
```


### 11.6 Type Traits

Cake provides compile-time type introspection functions that return boolean integer constant expressions:


**`_is_pointer(T)`** — true for pointers to objects or functions

**`_is_array(T)`** — true for array types

**`_is_function(T)`** — true for function types

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

<button onclick="Try(this)">try</button>


### 11.7 Object Lifetime Checks (Ownership)

Cake includes a static analysis system for tracking object ownership and lifetime, activated with `#pragma safety enable` or `-fanalyzer`.

**`_Owner`** — This pointer or variable is responsible for freeing the object.

**`_Opt`** — This pointer may be null (optional).

**`_View`** — This pointer borrows the object without owning it.

Full documentation: [ownership.html](ownership.html)

### 11.8 Pre-defined Macros

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
- `__attribute__`
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

*End of Cake C Compiler Reference Manual*