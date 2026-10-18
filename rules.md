# Cake annotations quick reference

Section 0 defines the terms used by the rules. Section 1 is the base contract, without qualifiers. Sections 2, 3 and 5 to 9 each start from a section 1 rule ("Same as 1.x, except") and list only what their qualifier changes; everything else in that rule still holds. Section 4 covers assignment and section 10 how qualifiers combine. Section 11 lists known limits and section 12 explains how the flow analysis works.

## 0. Definitions

- 0.1 **Valid**: the object and every object reachable from it are fully initialized, and their non-`_Opt` pointer members are non-null. Any member may be read without checks, except `_Opt` members. If it is not `const`, any member may also be written (4.1).
- 0.2 **Uninitialized**: the object holds no value and no resources. Members must be written before they are read (reading is a warning); there is nothing to move.
- 0.3 **File-scope objects**: a file-scope object is a parameter passed another way. Every rule for a parameter also holds for it. Its first read in a function starts as any value, like a parameter at entry: nothing set by another function is assumed, only the invariants of its type (a non-`_Opt` pointer is non-null). For example, at exit it may not point to a local of the function (1.1.5):

<!-- runnable -->

```c
#pragma safety enable

int* _Opt keep;

void f(int** p)
{
    int x = 0;
    *p = &x;
    keep = &x;
} //warning 31 'keep' points to an object whose lifetime has ended after the function returns
  //warning 31 'p' points to an object whose lifetime has ended after the function returns
```

## 1. No annotation

### 1.1 `void F(struct X* p)`

- **Implementation assumes**
  - 1.1.1 `p` is not null.
  - 1.1.2 `*p` is valid (0.1).
  - 1.1.3 `p` does not own `*p`; the function must not move `*p`. Owners reachable from it may be moved (see 1.1.6).
- **Implementation guarantees**
  - 1.1.4 `p` stays non-null throughout the implementation; null cannot be assigned to `p`.
  - 1.1.5 The values of `*p` and every object reachable from it may change, but `*p` is still valid.
  - 1.1.6 Ownership of `*p` does not change. Owners reachable from it may be moved, but at exit none is left moved: each is null or points to a new object.
- **Caller must pass**
  - 1.1.7 A non-null pointer to a valid object.
- **Caller assumes**
  - 1.1.8 `*p` is still valid (0.1) and still owned by the caller.
  - 1.1.9 `*p` and every object reachable from it may have been modified; pointers the caller held into reachable objects (e.g. a copy of `p->text`) are invalidated.
  - 1.1.10 What the caller knew about the state of `*p` and reachable objects before the call is lost; only the invariants of the type hold (e.g. a `_Opt` member tested non-null before the call may be null after it).

A member that is released must be set again (1.1.6):

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

The caller assumes the worst (1.1.9): `g` may have released `x.text`, so `p` may point to a deleted object.

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

### 1.2 `struct X* F(void)`

- **Implementation guarantees**
  - 1.2.1 The result is not null.
  - 1.2.2 The result does not own `*result`, but `*result` may own other objects through its `_Owner` members.
  - 1.2.3 `*result` is valid.
- **Caller assumes**
  - 1.2.4 The result can be used without a null check.
  - 1.2.5 The caller must not move `*result`; owners reachable from it may be moved or replaced, as in 1.1.3.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { int i; };

struct X* F(void);

void G(void)
{
    struct X* p = F();
    p->i = 1;  // ok: no null check needed
    free(p);   //warning 78 passing non-owner 'p' to an owner parameter
}
```

### 1.3 `struct X { char* text; };`

- 1.3.1 Readers assume `text` is not null.
- 1.3.2 Writers must not store null (except temporarily after `= {}`, see 4.1.9).

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
}

void g(struct person* p)
{
    p->name = 0;               //warning 35 null assigned to non-nullable
}
```

### 1.4 `void F(const struct X* p)`

Same as 1.1, except:

- **Implementation guarantees**
  - 1.4.1 `const` is deep: `*p` and every object reachable from it are not written (deep const is not checked yet).
  - 1.4.2 Owners reachable from `*p` are not moved, not even temporarily (1.1.3 and 1.1.6 do not apply).
- **Caller assumes**
  - 1.4.3 The caller's pointers into `*p` and reachable objects stay valid, and what the caller knew about their state still holds (1.1.9 and 1.1.10 do not apply).

Compare with the worst case sample in 1.1: with `const`, there is no warning.

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

## 2. `_Out`

### 2.1 `void F(_Out struct X* p)`

Same as 1.1, except:

- **Implementation assumes**
  - 2.1.1 `*p` is uninitialized (0.2).
- **Implementation guarantees**
  - 2.1.2 On every exit path, including error paths, `*p` and every object reachable from it are fully initialized to unknown values; `_Owner` members not created are set to null.
  - 2.1.3 The caller owns the `_Owner` members of `*p`.
- **Caller must pass**
  - 2.1.4 `*p` may be in any state (uninitialized, initialized, moved, ...), except that it cannot hold resources; otherwise it would be a leak.

2.1.5 `_Out` does not apply to the return value.

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

A missing member is reported (2.1.2):

<!-- runnable -->

```c
#pragma safety enable

struct point { int x; int y; };

void point_init(_Out struct point* p)
{
    p->x = 0;
} //warning 71 _Out parameter 'p->y' is not initialized at exit
```

Reading the input is reported (2.1.1):

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

## 3. `_Opt`

### 3.1 `void F(struct X* _Opt p)`

Same as 1.1, except:

- 3.1.1 The caller may pass null; the implementation checks `p` before use.

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

### 3.2 `struct X* _Opt F(void)`

Same as 1.2, except:

- 3.2.1 The result may be null; the caller checks it before use.

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

### 3.3 `void F(_Opt struct X* p)`

Same as 1.1, except:

- 3.3.1 Any pointer member of `*p` and every object reachable from it may be null, even members declared without `_Opt`.
- 3.3.2 The caller may pass a possibly incomplete object, e.g. right after `= {}`.

3.3.3 Candidate for removal: where it is needed, `_Dtor` already implies it.

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

### 3.4 `struct X { char* _Opt text; };`

Same as 1.3, except:

- 3.4.1 Readers check before use.
- 3.4.2 Writers may store null.

<!-- runnable -->

```c
#pragma safety enable

struct person {
    const char* name;
    const char* _Opt nickname; // may be null
};

void print(const char* s);

void f(const struct person* p)
{
    print(p->nickname);        //warning 35 may be null
    if (p->nickname)
        print(p->nickname);    // ok
}

void g(struct person* p)
{
    p->nickname = 0;           // ok
}
```

## 4. Assignment

### 4.1 `a = b`

- **Destination `a` must be**
  - 4.1.1 `a` and every `_Owner` member of `a` are empty (uninitialized, null or moved); otherwise the old resource leaks.
- **Source `b` must be**
  - 4.1.2 Initialized.
  - 4.1.3 Not null when `a` is not `_Opt`.
  - 4.1.4 `b` and every `_Owner` member of `b` were not moved before.
- **After the assignment**
  - 4.1.5 `a` has the value and the null state of `b`.
  - 4.1.6 `_Owner = _Owner`: ownership moves to `a`; `b` is moved.
  - 4.1.7 non-owner `= _Owner`: `b` keeps ownership; `a` is a view. After the owner is released, using `a` is detected.
  - 4.1.8 `_Owner =` non-owner: not allowed.

4.1.9 `struct X x = {}` may leave non-`_Opt` members null; they must be set before the object is used.

4.1.10 Passing an argument to a parameter follows the same rules, with the parameter as `a`.

4.1.11 Qualified parameters change these rules: `_Out` (2.1.4 replaces 4.1.1), `_View` (8.1.2 replaces 4.1.6), `_Dtor` (5.1.3 replaces 4.1.3 for members).

Assignment moves ownership (4.1.6):

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

After a move, the old owner cannot be used (4.1.4):

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

A view of a released owner (4.1.7):

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

`= {}` (4.1.9):

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

## 5. `_Dtor`

### 5.1 `void F(_Dtor struct X* p)`

Same as 1.1, except:

- **Implementation assumes**
  - 5.1.1 As in 3.3.1, any pointer member of `*p` may be null, even members declared without `_Opt`. Justification: the destructor must also destroy partially created objects.
- **Implementation guarantees**
  - 5.1.2 On every exit path, every `_Owner` member of `*p` has been moved. Moved members do not need to be set again.
- **Caller must pass**
  - 5.1.3 As in 3.3.2, an object that may be partially constructed (e.g. right after `= {}`, or a construction that failed halfway), but nothing in `*p` or reachable from it may be uninitialized; members not yet constructed must be null.
- **Caller assumes**
  - 5.1.4 `*p` is uninitialized (0.2).
  - 5.1.5 The caller still owns the memory of `*p` and may move it (e.g. `free(p)`) or reuse it (e.g. with `_Out`).
  - 5.1.6 For 4.1.1, `*p` counts as empty: its members may be assigned without moving first.

5.1.7 `_Dtor` cannot be used with `const`.

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

Forgetting a member (5.1.2):

<!-- runnable -->

```c
#pragma safety enable

struct X { char* _Owner _Opt text; };

void x_destroy(_Dtor struct X* p)
{
} //warning 29 'p->text' still owns its resource
```

After the call, the members cannot be used (5.1.4):

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

Partially constructed objects (5.1.1, 5.1.3): if construction fails halfway, some members are still zero, and the destructor must accept that object too.

<!-- runnable -->

```c
#pragma safety enable

_Clear void* _Owner _Opt calloc(unsigned long n, unsigned long size);
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

## 6. `_Clear`

### 6.1 `void F(_Clear struct X* p)`

Same as 1.1, except:

- **Implementation guarantees**
  - 6.1.1 On every exit path, every `_Owner` member of `*p` has been moved.
  - 6.1.2 On every exit path, every member of `*p` is zero.
- **Caller assumes**
  - 6.1.3 Every member of `*p` is zero; the object can be used again (e.g. a member may be assigned without moving first).

6.1.4 `_Clear` cannot be used with `const`.

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

A member that is not zero at exit is reported (6.1.2):

<!-- runnable -->

```c
#pragma safety enable

struct X { char* _Owner _Opt text; int n; };

void x_clear(_Clear struct X* p)
{
    p->n = 0;
} //warning 69 _Clear parameter 'p' pointee (.text) is not zero at exit
```

After the call, the object can still be used (6.1.3):

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

### 6.2 `_Clear struct X* F(void)`

Same as 1.2, except (1.2.3 and 1.2.4 do not apply):

- 6.2.1 Every member of `*result` is zero, like memory from `calloc`; non-`_Opt` members are null until set.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
void use(int i);

struct X { int a; int b; };

_Clear struct X* _Owner _Opt x_create(void);

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

## 7. `_Uninitialized`

### 7.1 `_Uninitialized struct X* F(void)`

Same as 1.2, except (1.2.3 and 1.2.4 do not apply):

- 7.1.1 `*result` is uninitialized (0.2), like memory from `malloc`.

7.1.2 Note: so far this has been used only on `malloc` and similar functions.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);
void use(int i);

struct X { int a; int b; };

_Uninitialized struct X* _Owner _Opt x_alloc(void);

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

## 8. `_View`

### 8.1 `void F(_View struct X x)`

Same as `void F(struct X x)`, except:

- 8.1.1 `x` does not own its `_Owner` members; the function must not move them.
- 8.1.2 Nothing is moved; the caller still owns the `_Owner` members of the argument.

8.1.3 Without `_View` (`void F(struct X x)`): the `_Owner` members of the argument are moved to the function.

8.1.4 Note: in practice `_View` has been rarely used, so it is a candidate for removal.

Without `_View` (8.1.3):

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

With `_View` (8.1.2):

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

## 9. `_Owner`

### 9.1 `void F(struct X* _Owner p)`

Same as 1.1, except:

- **Implementation assumes**
  - 9.1.1 `p` owns `*p`.
- **Implementation guarantees**
  - 9.1.2 On every exit path `p` has been moved (e.g. freed, or passed to another `_Owner`).
- **Caller assumes**
  - 9.1.3 The argument is moved; it cannot be read (1.1.8 to 1.1.10 do not apply).

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

A pointer to a struct owns **two resources**: the memory and the object in it. `free(void*)` releases only the memory, so the object must be released first.

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

### 9.2 `struct X* _Owner F(void)`

Same as 1.2, except:

- 9.2.1 The result owns `*result`; the caller must move it (1.2.2 and 1.2.5 do not apply).

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

### 9.3 `struct X { char* _Owner text; };`

Same as 1.3, except:

- 9.3.1 The object owns `*text`; destroying the object moves `text` (see 5.1.2).
- 9.3.2 Writers follow 4.1.1: `text` must be empty before it is assigned.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void f(struct X* p)
{
    p->text = strdup("a"); //warning 26 'p->text' discarded without releasing it first
}

void x_destroy(_Dtor struct X* p)
{
} //warning 29 'p->text' still owns its resource
```

## 10. Combinations

10.1 Qualifiers combine; each one adds its own contract and they do not change each other.

10.2 Example: `_Uninitialized struct X* _Owner _Opt F(void)` is 3.2 (`_Opt`: check for null) + 9.2 (`_Owner`: the caller must move it) + 7.1 (`*result` is uninitialized).

10.3 Example: `_Out _Clear struct X* p` is 2.1 (`*p` is uninitialized at entry, so nothing is released) + 6.1 (every member is zero at exit). If the caller passes an object that still owns something, the warning is at the call site:

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

## 11. Limits

### 11.1 `realloc`

Some contracts cannot be expressed with these annotations yet; they will need a richer way to describe contracts in the future. The most common is `realloc`: ownership of `p` moves only if the call succeeds.

```c
void* _Owner _Opt realloc(void* _Opt p, size_t size);
```

`p` is declared as non-owner, so after a successful call cake still sees `p` as the owner. Suppress the warning where the new pointer replaces the old one:

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

`//lint N` marks the line where warning `N` is expected, and silences it. If the expected warning does not happen, that is also a warning. `//lint` is the safest way to silence a warning because it does not interfere with the analysis. It is not a command that overrides a rule.

### 11.2 Contracts still missing

The annotations do not describe everything the analysis needs to know. Some bugs can only be found with new kinds of contracts. In the sample below, `id` returns its argument, but its declaration does not say so. Cake cannot tell that `q` points to the object freed through `p`, so the use after free is not reported. A contract saying "the result points to the same object as `q`" would let the analysis follow the pointer through the call.

```c
#pragma safety enable
void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

int* id(int* q) { return q; }

int f(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (!p) return 0;
    int* q = id(p);
    free(p);
    return *q;   // use after free
}
```

## 12. Flow analysis

- **One function at a time.** Calls are not followed; a call is checked against the declaration of the function. That is why annotations matter: they are the contract between caller and implementation.
- **A state per object.** For each variable, member and pointed object, cake keeps the set of states it may have: null, not null, uninitialized, moved, lifetime ended, ...
- **Each state keeps its origin.** Cake remembers where each state came from, so a warning points to the line that caused it (for example, the `free` that ended the lifetime).
- **Warnings are questions.** "Can `p` be null here?", "does `s` still own its resource at the end?". If any state says yes, the warning is reported.

`static_debug(x)` prints the state of `x` at that point.

### 12.1 The domino effect: fixing a leak demo

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
