# Cake annotations quick reference

## Introduction by example

### 1. `_Out`

`_Out`: the function writes the object; it does not read it.

<!-- runnable -->

```c
#pragma safety enable

struct point { int x; int y; };

void point_init(_Out struct point* p)
{
    p->x = 0;
    p->y = 0;
}

int main()
{
    struct point pt;  // uninitialized
    point_init(&pt);  // ok: _Out accepts uninitialized storage
    static_debug(pt); // pt.x and pt.y are initialized
}
```

A missing member is reported:

<!-- runnable -->

```c
#pragma safety enable

struct point { int x; int y; };

void point_init(_Out struct point* p)
{
    p->x = 0;
} //warning 71 _Out parameter 'p->y' is not initialized at exit
```

Reading the input is reported:

<!-- runnable -->

```c
#pragma safety enable

struct point { int x; int y; };

void point_init(_Out struct point* p)
{
    int i = p->x; //warning 30 assigning a possibly uninitialized object 'p->x'
    p->x = 0;
    p->y = 0;
}
```

### 2. `_Opt`

`_Opt`: the pointer may be null. Check before use.

<!-- runnable -->

```c
#pragma safety enable

struct point { int x; int y; };

struct point* _Opt find_point(int id);

int main()
{
    struct point* _Opt p = find_point(1);
    p->x = 1;     //warning 33 possible null dereference
    if (p)
        p->x = 1; // ok, narrowed to non-null
}
```

On a parameter: the caller may pass null.

<!-- runnable -->

```c
#pragma safety enable

struct point { int x; int y; };

void point_print(const struct point* _Opt p)
{
    if (p == 0) return;
    int x = p->x;   // ok, p is non-null here
}

int main()
{
    point_print(0); // ok
}
```

### 3. `_Opt` on struct members

Same rule for members: without `_Opt` the member is non-null.

<!-- runnable -->

```c
#pragma safety enable

struct person {
    const char* name;          // never null
    const char* _Opt nickname; // may be null
};

void print(const char* s);

void f(const struct person* p)
{
    print(p->name);            // ok
    print(p->nickname);        //warning 35 may be null
    if (p->nickname)
        print(p->nickname);    // ok
}

void g(struct person* p)
{
    p->nickname = 0;           // ok
    p->name = 0;               //warning 35 null assigned to non-nullable
}
```

Exception: `= {}` may leave a non-`_Opt` member temporarily null.
It must be set before the object is used.

<!-- runnable -->

```c
#pragma safety enable

struct person {
    const char* name;
    const char* _Opt nickname;
};

void use(const struct person* p);

void f1(void)
{
    struct person p = {}; // ok, p.name is null for now
    p.name = "Ana";
    use(&p);              // ok
}

void f2(void)
{
    struct person p = {};
    use(&p);              //warning 35 'p.name' may be null
}
```

`_Opt` on the pointed object: `_Opt struct X* p` means every member of `*p`
may be null, even members declared without `_Opt`.

<!-- runnable -->

```c
#pragma safety enable

struct X { char* text; };

void use(char* s);

void f(_Opt struct X* p)
{
    use(p->text);     //warning 35 'p->text' may be null
    if (p->text)
        use(p->text); // ok
}
```

### 4. `_Owner`

`_Owner`: the pointer owns the resource and must release it.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

void f(void)
{
    char* _Owner _Opt s = strdup("a");
    free(s); // ok, released
}
```

Forgetting to release:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);

void f(void)
{
    char* _Owner _Opt s = strdup("a");
} //warning 29 's' still owns its resource
```

Assignment moves ownership:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

void f(void)
{
    char* _Owner _Opt s = strdup("a");
    char* _Owner _Opt t = s; // t is the owner now
    free(t);
}
```

After a move, the old owner cannot be used:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

void f(void)
{
    char* _Owner _Opt s = strdup("a");
    free(s);
    free(s); //warning 32 's' was moved
}
```

A pointer to a struct owns **two resources**: the memory and the object in it.
`free(void*)` releases only the memory, so the object must be released first.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void x_delete_bad(struct X* _Owner p)
{
    free(p);       //warning 29 'p->text' still owns its resource
}

void x_delete(struct X* _Owner p)
{
    free(p->text); // the object
    free(p);       // the memory
}
```

### 5. `_Owner` parameter

The function receives ownership and must release it.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

void consume(char* _Owner _Opt p)
{
    free(p); // ok
}

void consume_bad(char* _Owner _Opt p)
{
}            //warning 29 'p' still owns its resource
```

### 6. `_Owner` return

The caller receives ownership.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

char* _Owner _Opt make(void)
{
    return strdup("a");
}

void f(void)
{
    char* _Owner _Opt s = make();
    free(s); // ok
}
```

Ignoring the result leaks it:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt make(void);

void f(void)
{
    make(); //warning 9 ignoring the result of _Owner type
}
```

### 7. Pointers to deleted objects

A non-owner pointer does not own the object, but cake still knows what it points
to. After the owner is released, using the other pointer is detected.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { int i; };

void f(struct X* _Owner p)
{
    struct X* view = p;
    free(p);
    view->i = 1; //warning 31 lifetime of 'view->i' has ended
}
```

### 8. Owner and non-owner conversions

| Assignment | Result |
|---|---|
| `owner = owner` | ok, ownership moves |
| `non-owner = owner` | ok, the owner keeps ownership |
| `owner = non-owner` | warning: assigning non-owner to an owner |

The same rules apply to arguments passed to parameters.

### 9. `_Dtor`

`_Dtor`: the function releases the members of the object, but not its memory.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void x_destroy(_Dtor struct X* p)
{
    free(p->text);
}

void f(void)
{
    struct X x = {};
    x.text = strdup("a");
    x_destroy(&x); // ok, x.text released
}
```

Forgetting a member:

<!-- runnable -->

```c
#pragma safety enable

struct X { char* _Owner _Opt text; };

void x_destroy(_Dtor struct X* p)
{
} //warning 29 'p->text' still owns its resource
```

After the call, the members cannot be used:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void x_destroy(_Dtor struct X* p);

void f(void)
{
    struct X x = {};
    x.text = strdup("a");
    x_destroy(&x);
    free(x.text); //warning 30 possibly uninitialized 'x.text'
}
```

`_Dtor` implies `_Opt`: inside the destructor every member may be null, even
members declared without `_Opt`. This is because objects can be partially
constructed. If construction fails halfway, some members are still zero, and the
destructor must accept that object too:

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X {
    char* _Owner name;
    char* _Owner email;
};

void x_destroy(_Dtor struct X* p)
{
    free(p->name);    // ok, may be null
    free(p->email);   // ok, may be null
}

struct X* _Owner _Opt x_create(const char* name, const char* email)
{
    struct X* _Owner _Opt p = calloc(1, sizeof * p);
    if (p == 0)
        return 0;

    char* _Owner _Opt s = strdup(name);
    if (s == 0)
    {
        free(p);
        return 0;
    }
    p->name = s;

    s = strdup(email);
    if (s == 0)
    {
        x_destroy(p); // ok, p->email is still null
        free(p);
        return 0;
    }
    p->email = s;

    return p;
}
```

### 10. Objects visible to the caller

Objects the caller can see after the call cannot be left messed up. A member that
is released must be set again (to `0` or a new value). `_Dtor` is the exception.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void f(struct X* p)
{
    free(p->text);
}                          //warning 72 'p->text' was moved but never reassigned

void g(struct X* p)
{
    free(p->text);
    p->text = 0;           // ok
}

void h(struct X* p)
{
    free(p->text);
    p->text = strdup("b"); // ok
}
```

### 11. Worst case scenario

`g` receives a non-const pointer, so cake assumes the worst: `g` may have
released `x.text`. `p` may point to a deleted object.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

struct X f();
void g(struct X* p);

int main()
{
    struct X x = f();
    char* _Opt p = x.text;
    g(&x);
    if (p)
    {
        char c = *p; //warning 31 lifetime of '*p' has ended
    }
    free(x.text);
}
```

Fix: read `x.text` again after the call.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

struct X f();
void g(struct X* p);

int main()
{
    struct X x = f();
    char* _Opt p = x.text;
    g(&x);
    p = x.text;      // read again
    if (p)
    {
        char c = *p; // ok
    }
    free(x.text);
}
```

If `g` does not change `x`, make the parameter `const`. Then there is no warning.

Note: the analysis assumes a "deep const": nothing reachable from `x` changes,
including the text `x.text` points to. C `const` itself does not protect that.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

struct X f();
void g(const struct X* p);

int main()
{
    struct X x = f();
    char* _Opt p = x.text;
    g(&x);
    if (p)
    {
        char c = *p; // ok
    }
    free(x.text);
}
```

### 12. `_Clear`

`_Clear` on a parameter works like `_Dtor`, but every member is left zero.
The function releases what it owns, then sets every member to zero.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; int n; };

void x_clear(_Clear struct X* p)
{
    free(p->text);
    p->text = 0;
    p->n = 0;
}
```

A member that is not zero at exit is reported:

<!-- runnable -->

```c
#pragma safety enable

struct X { char* _Owner _Opt text; int n; };

void x_clear(_Clear struct X* p)
{
    p->n = 0;
} //warning 69 _Clear parameter 'p' pointee (.text) is not zero at exit
```

After the call, the object can still be used, and all of its members are zero:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; int n; };

void x_clear(_Clear struct X* p);

void f(void)
{
    struct X x = {};
    x.text = strdup("a");
    x_clear(&x);          // x.text released, x is all zero
    x.text = strdup("b"); // ok, x.text is zero
    free(x.text);
}
```

On a **return value**, `_Clear` means the same thing: the object that is returned
is all zero, like memory from `calloc`.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
void use(int i);

struct X { int a; int b; };

struct X* _Owner _Opt _Clear x_create(void);

void f(void)
{
    struct X* _Owner _Opt p = x_create();
    if (p)
    {
        use(p->a); // ok, p->a is zero
        free(p);
    }
}
```

### 13. `_Out _Clear`

`_Out _Clear`: the function receives uninitialized storage and leaves every member zero.
Because the input is treated as uninitialized, the function does not release anything.
If the caller passes an object that still owns something, the warning is at the call site:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);

struct X { char* _Owner _Opt text; int n; };

void x_init(_Out _Clear struct X* p);

void f(void)
{
    struct X x; // uninitialized
    x_init(&x); // ok, x is all zero
}

void g(void)
{
    struct X x = {};
    x.text = strdup("a");
    x_init(&x); //warning 26 x.text is overwritten without being released
}
```

### 14. The domino effect: fixing a leak demo

Code with a leak compiles clean when the checks are disabled.

<!-- runnable -->

```c
//#pragma safety enable
#include <stdlib.h>
#include <string.h>

struct person { char* name; };

void person_set_name(struct person* p, const char* name)
{
    p->name = strdup(name);
}

int main()
{
    struct person p = {};
    person_set_name(&p, "Ana");
    person_set_name(&p, "Bia");
    
}
```


### 15. `_View`

Passing a struct by value moves its `_Owner` members.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void show(struct X x);

void f(void)
{
    struct X x = {};
    show(x);      // x.text moved to show
    free(x.text); //warning 32 'x.text' was moved
}
```

`_View`: the struct does not own its members. Nothing is moved.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void show(_View struct X x);

void f(void)
{
    struct X x = {};
    show(x);      // not moved
    free(x.text); // ok, x is still the owner
}
```

Note: in practice `_View` has been rarely used, so it is a candidate for removal.

### 16. `_Uninitialized`

`_Uninitialized` on a return value: the object that is returned is uninitialized,
like memory from `malloc`. Members must be written before they are read.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
void use(int i);

struct X { int a; int b; };

struct X* _Owner _Opt _Uninitialized x_alloc(void);

void f(void)
{
    struct X* _Owner _Opt p = x_alloc();
    if (p)
    {
        p->a = 1;
        use(p->a); // ok, written first
        use(p->b); //warning 30 'p->b' is uninitialized
        free(p);
    }
}
```

Note: in practice `_Uninitialized` has been used almost only on `malloc` and similar functions.

### 17. Limits: `realloc`

Some contracts cannot be expressed with these annotations yet; they will need a
richer way to describe contracts in the future. The most common is `realloc`:
ownership of `p` moves only if the call succeeds.

```c
void* _Owner _Opt realloc(void* _Opt p, size_t size);
```

`p` is declared as non-owner, so after a successful call cake still sees `p` as
the owner. Suppress the warning where the new pointer replaces the old one:

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt realloc(void* _Opt p, unsigned long size);
void free(void* _Owner _Opt p);

void f(int* _Owner _Opt p)
{
    int* _Owner _Opt q = realloc(p, 10 * sizeof(int));
    if (q == 0)
    {
        free(p); // failed: p still owns the memory
        return;
    }
    p = q;       //lint 26 (realloc)
    free(p);
}
```

`//lint N` marks the line where warning `N` is expected, and silences it.
If the expected warning does not happen, that is also a warning.
`//lint` is the safest way to silence a warning because it does not interfere
with the analysis. It is not a command that overrides a rule.

### 18. Flow analysis

- **One function at a time.** Calls are not followed; a call is checked against
  the declaration of the function. That is why annotations matter: they are the
  contract between caller and implementation.
- **A state per object.** For each variable, member and pointed object, cake
  keeps the set of states it may have: null, not null, uninitialized, moved,
  lifetime ended, ...
- **Each state keeps its origin.** Cake remembers where each state came from,
  so a warning points to the line that caused it (for example, the `free`
  that ended the lifetime).
- **Warnings are questions.** "Can `p` be null here?", "does `s` still own its
  resource at the end?". If any state says yes, the warning is reported.

`static_debug(x)` prints the state of `x` at that point.

### 19. Not covered here

- The guarantees that are still missing: cases where the analysis fails.
- The details of how the flow analysis works.
- Other combinations and uses of each annotation.
