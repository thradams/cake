
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

### 10.6 `if` Declarations

A declaration can appear in the initializer clause of an `if` statement, scoping the declared variable to the entire `if`/`else` chain.

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
}
```


Reference: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3388.htm

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

