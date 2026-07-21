
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
