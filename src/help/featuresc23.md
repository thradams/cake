
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

### 9.14 Enhancements to Enumerations (Typed Enums)

<!-- runnable -->

```c
enum X : short { A };

int main() {
    enum X x = A;
}
```


> **Note** TODO Missing some details.

Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm

### 9.15 Attributes (`[[...]]`)

Cake supports the C23 double-bracket attribute syntax. Recognized standard attributes:

**`[[fallthrough]]`** *(Partial — parsed; enforcement pending)*  
Suppresses the fallthrough diagnostic on a switch case.

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


<!-- runnable -->

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



> **Note** Some details are not implemented yet.

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

<!-- runnable -->

```c
void F(int* p) {}

int main() {
    F((static int[]){1, 2, 3, 0});
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3038.htm
