

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



To disable this built-in behavior and use a standard macro instead, pass `-disable-assert`.

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
This is the pragma used in `cakeconf.h` to declare system and project include directories.

```c
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt"
#pragma dir "./openssl/include"
```

### 11.5 `offsetof` Operator

In Cake, `offsetof` is a built-in operator rather than a macro.  Similar of GCC `__builtin_offsetof`.
This allows its use in constant expressions and avoids the undefined 
behavior associated with traditional macro implementations.

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
