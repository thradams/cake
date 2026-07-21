
## 7. C99 Features

### 7.1 `restrict` Pointers

`restrict` is parsed and type-checked but stripped from the generated C89
output.

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


The size of a structure with a flexible array member is computed as if the
member were omitted, except that additional trailing padding may be present.

### 7.4 `static` and Type Qualifiers in Array Declarators

The `static` keyword in array declarators is parsed and checked. Passing `NULL`,
`nullptr`, or an array smaller than the declared minimum is a diagnostic.

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

Cake converts hexadecimal floating-point literals to decimal representation
using `strtod` followed by `snprintf`. This conversion may introduce minor
precision loss.

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


Inline functions in Cake are equivalent to static, since Cake does not currently
perform function inlining.

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
