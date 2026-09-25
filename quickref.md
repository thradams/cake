# Cake annotations quick reference

Every annotation is a **contract** with two sides:

- the **caller**: what it must provide, and what it gets back;
- the **implementation**: what it may assume, and what it must guarantee at *every* exit.

Cake checks both sides: callers at the call site, implementations at every return path.

---

## The invisible contract

### Parameter: `void f(struct X * p)`

**Caller**

- must pass a non-null `p`;
- must pass `*p` with every member initialized;
- keeps ownership: after the call `*p` is still complete and the caller still
  has to release its owners.

**Implementation**

- assumes `p != NULL` ;
- assumes every member of `*p` is initialized;
- can read and modify `*p`;
- cannot mess `*p` ; zx fully initialized at **every** exit. If it releases an owner
  member, it must put a valid value back before returning, including on early returns;
- must not consume `*p`: only `_Dtor` may leave the caller's object consumed.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct person { char* _Owner _Opt name; };

void set_name(struct person* p, const char* name)
{
    free(p->name);
    char* _Owner _Opt tmp = strdup(name);
    if (tmp == 0) return; //lint 72 p->name released but never reassigned
    p->name = tmp;
}
```

Fix: set `p->name = 0;` before the early return (the member is `_Opt`), or
allocate first and free after.

### Return value: `struct X f(void)`

**Implementation**

- must return an object with every member initialized;
- transfers its `_Owner` members to the caller, so it must not free them first.

**Caller**

- may assume the result is complete;
- must release its owner members, or move them somewhere.

<!-- runnable -->

```c
#pragma safety enable

struct X { char* text; };

struct X f1(void)
{
    struct X x;
    return x; //lint 30 returning uninitialized 'x.text'
}

struct X f2(void)
{
    struct X x = {};
    return x; //lint 35 returning possible null pointer 'x.text'
}
```

A returned pointer `struct X * f(void)` follows the same default: non-null, and
the pointee is initialized. Returns that differ from the default are described
with annotations: `_Opt`, `_Owner`, `_Uninitialized`, `_Clear`.

---

## `_Opt`

### On a pointer

The pointer may be null. Without `_Opt` a pointer is non-null.

**Caller**: may pass null to an `_Opt` parameter; must check an `_Opt` result before using it.
**Implementation**: must check an `_Opt` parameter before dereferencing it; may return null.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);
void print(const char* s);

int main()
{
    char* _Owner _Opt s = strdup("a");
    print(s);          //lint 35 s may be null
    if (s) print(s);   // ok, narrowed to non-null
    free(s);
}
```

Rules of thumb: a variable initialized `= NULL` is `_Opt`; a parameter the body
null-checks is `_Opt`.

### On the struct (pointee)

`_Opt struct X *` means the object may be only partially built: every pointer
member is treated as possibly null, even members declared non-null. Used by
delete functions that must accept an object whose initialization failed halfway.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner text; }; // text normally non-null

void x_delete(_Opt struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->text); // p->text treated as possibly null
        free(p);
    }
}
```

### Zero-initialized objects are `_Opt` objects

`= {}` and `calloc` both produce an object whose members are all zero, including
members declared non-null. Such an object is in the `_Opt` state: that is
allowed, but it is not yet a complete object under the default contract.

A zero-initialized object is valid to hold, to finish initializing, and to
destroy, but members declared non-null must be assigned before it is used as a
complete object (passed to a default parameter or returned).

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt p);

struct X { char* text; }; // text is non-null

void use(struct X* p);             // default: *p must be complete

void f(void)
{
    struct X x = {};               // ok: x.text == 0, x is an _Opt object
    static_debug(x);               // text == null
    use(&x);                       //lint 35 x.text is null
}

void g(void)
{
    struct X* _Owner _Opt p = calloc(1, sizeof * p); // p->text == 0
    static_debug(p);
    if (p)
    {
        p->text = "a";             // now complete
        use(p);                    // ok
        free(p);
    }
}
```

---

## `_Owner`

The reference is responsible for the lifetime of the object. Assignment, or
passing to an `_Owner` parameter, **moves** ownership. An owner must be
released or moved before it goes out of scope.

It can go on pointers, struct members and return types. Its placement follows
the same rules as `const`: it belongs to the storage holding the reference,
not to the pointed-to type.

**`_Owner` parameter**
- Caller: gives the object away; after the call the argument is moved and must not be used.
- Implementation: receives a complete object and must release or move it at every exit.

**`_Owner` return**
- Implementation: returns an object it gives away.
- Caller: must release it or move it.

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt malloc(unsigned long n); // returns ownership
void free(void* _Owner _Opt p);            // takes ownership

int main()
{
    void* _Owner _Opt p = malloc(1);
    void* _Owner _Opt p2 = p;   // moved: p is no longer the owner
    static_debug(p);
    free(p2);                   // ok
    free(p);                    //lint 32 p was moved
}
```

---

## Assignment rules

The same rules apply to `=`, to initialization, to passing an argument
(argument -> parameter) and to `return` (expression -> return type).

### `_Opt`

| Destination `=` source | Result |
|---|---|
| `_Opt` = non-null | ok (widening) |
| `_Opt` = `_Opt` | ok |
| non-null = non-null | ok |
| non-null = `_Opt` | warning 35, unless a null check has narrowed the source |

<!-- runnable -->

```c
#pragma safety enable

char* _Opt get_opt(void);
char* get_non_null(void);

void f(void)
{
    char* _Opt a = get_non_null(); // ok: non-null -> _Opt (widening)
    char* b = get_opt();           //lint 35 _Opt -> non-null (may be null)

    char* _Opt c = get_opt();
    if (c)
    {
        char* d = c;               // ok: c was checked, narrowed to non-null
        (void)d;
    }
    (void)a;
    (void)b;
}
```

### `_Owner`

| Destination `=` source | Result |
|---|---|
| `_Owner` = `_Owner` | ok: **move**, the source is now moved |
| `_Owner` = non-owner | warning 78: cannot make an owner from something not owned |
| non-owner = `_Owner` variable | ok: the destination is a **view**, the source is still the owner |
| non-owner = `_Owner` temporary (e.g. `malloc(1)`) | warning 79: nobody owns the result, so it leaks |

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

void f(void)
{
    int i = 0;
    int* _Owner _Opt a = &i;        //lint 78 non-owner -> _Owner
    free(a);

    void* _Opt b = malloc(1);       //lint 79 temporary owner -> non-owner (leak)
    (void)b;

    void* _Owner _Opt p = malloc(1);
    void* _Opt view = p;            // ok: non-owner copy of an owner variable is a view
    void* _Owner _Opt q = p;        // ok: _Owner -> _Owner moves; p is now moved
    (void)view;
    free(q);
}
```

A view must not outlive its owner: after `free(q)` above, `view` points to an
object whose lifetime has ended.

---

## `_View`

Used **only on a struct object**, never on a pointer (plain pointers already are
views; `int * _View p` is an error). For that variable, `_Owner` is removed from
every member, so the struct can be copied or passed without moving ownership.

**Caller**: keeps ownership.
**Implementation**: may read the members but must not release or move them.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; };

void show(_View struct X x) { /* reads x, owns nothing */ }

int main()
{
    struct X x = {};
    show(x);                 // x not moved
    _View struct X x2 = x;   // not moved
    free(x.text);            // x is still the owner
}
```

---

## `_Out`

On a **parameter's pointee**. The constructor/init contract.

**Caller**
- may pass uninitialized storage;
- after the call the object is fully initialized and the caller owns its contents.

**Implementation**
- may assume **nothing** about `*p`: it must not read or free its members;
- must initialize **every** member (not only owners) at every exit.

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; int i; };

void x_init(_Out struct X* p)
{
    p->text = 0;
    p->i = 0;
}

void x_init_partial(_Out struct X* p)
{
    p->text = 0;
} //lint 71 _Out parameter 'p->i' is not initialized at exit

int main()
{
    struct X x;   // uninitialized is fine
    x_init(&x);
    static_debug(x);  // every member initialized
    free(x.text);
}
```

---

## `_Dtor`

On a **parameter's pointee**. The inverse of `_Out`; the destructor contract.

**`_Dtor` implies `_Opt` by design:** `_Dtor struct X *` means
`_Dtor _Opt struct X *`. Every member is possibly null, whatever its declared
nullability. So a destructor accepts any object that may exist, including one
that is zero-initialized (`= {}`, `calloc`) or that failed halfway through
construction.

**Caller**
- must pass an object (complete or in the `_Opt` state);
- after the call the object is consumed: it must not read it or release it again.

**Implementation**
- treats every member as `_Opt`, so guards them before use (`free(NULL)` is fine);
- must release or move every `_Owner` member at every exit;
- does **not** need to zero anything.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner text; }; // non-null, but _Opt inside a _Dtor

void x_destroy(_Dtor struct X* p)
{
    free(p->text); // obligation fulfilled
}

int main()
{
    struct X x = {};  // _Opt object: x.text == 0
    x_destroy(&x);    // ok: _Dtor accepts it
    static_debug(x);  // consumed
}
```

`_Dtor` frees the **members**. A function that frees the members **and** the
object is a `_delete` taking `_Owner`:

```c
void x_delete(_Opt struct X* _Owner _Opt p)
{
    if (p)
    {
        x_destroy(p); // moves *p's contents
        free(p);      // releases storage
    }
}
```

---

## `_Clear`

### On a parameter's pointee: the reset contract

**Caller**
- passes a fully initialized object (it may hold owners);
- after the call every member is zero and nothing is owned.

**Implementation**
- may assume every member is initialized, so it may read them;
- must release owner members (it receives a live object);
- must leave **every** member == 0 at **every** exit. An early return that skips
  the zeroing breaks the contract.

Constraints: the pointee cannot be `const`, and `_Clear` goes on the pointee,
not the pointer (`struct s _Clear p` is an error).

<!-- runnable -->

```c
#pragma safety enable

void free(void* _Owner _Opt p);

struct S { char* _Owner _Opt text; int n; };

void s_reset(_Clear struct S* p)
{
    free(p->text);
    p->text = 0;
    p->n = 0;
}

void s_reset_forgot(_Clear struct S* p)
{
    free(p->text);
    p->text = 0;
} //lint 69 _Clear parameter 'p->n' is not zero at exit
```

### On a return's pointee

The returned memory is already zeroed, like `calloc`:

```c
void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
```

---

## `_Out _Clear` together

The zero-init contract for fresh storage.

**Caller**
- may pass uninitialized storage;
- must **not** hold live owners inside it: they would be overwritten without
  being released (warning);
- after the call every member is zero.

**Implementation**
- may assume **nothing** (as with `_Out`): must not read or free members;
- must set **every** member to zero at every exit.

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X { char* _Owner _Opt text; int i; };

void x_zero(_Out _Clear struct X* p)
{
    p->text = 0;
    p->i = 0;
}

void bad(void)
{
    struct X x = {};
    x.text = strdup("a");
    x_zero(&x); //lint 26 discards _Owner 'x.text' without releasing it first
}

void ok(void)
{
    struct X x;  // uninitialized is fine
    x_zero(&x);
    static_debug(x);  // every member zero
}
```

| | Entry | Exit |
|---|---|---|
| `_Clear` | initialized, may hold owners (callee frees them) | all zero |
| `_Out _Clear` | may be uninitialized; no live owners | all zero |

Use `_Clear` for *reset* functions, `_Out _Clear` for *init-to-zero* functions.

---

## `_Uninitialized`

On a **return's pointee**. The returned memory is raw, like `malloc`.

**Implementation**: returns storage without initializing it.
**Caller**: must write each member before reading it.

<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt _Uninitialized malloc(unsigned long n);
void free(void* _Owner _Opt p);
void print(const char* s);

struct X { char* text; };

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof * p);
    static_debug(p);
    if (p)
    {
        print(p->text);  //lint 30 p->text is uninitialized
        p->text = "a";
        print(p->text);  // ok
        free(p);
    }
}
```

---

## Summary

### Parameter pointee

| Annotation | Caller provides on entry | Implementation may assume | Implementation must guarantee at exit | Caller gets after the call |
|---|---|---|---|---|
| *(none)* | fully initialized | every member readable | fully initialized, owners still valid | same object, still owns it |
| `_Out` | anything | nothing: no read, no free | every member initialized | initialized object, owns it |
| `_Dtor` (= `_Dtor _Opt`) | complete or `_Opt` object | every member `_Opt` | every `_Owner` member released | consumed object |
| `_Clear` | fully initialized | every member readable | every member == 0 (owners freed) | all-zero object |
| `_Out _Clear` | anything, no live owners | nothing: no read, no free | every member == 0 | all-zero object |
| `_Owner` pointer | owned object | owned, fully initialized | object released or moved | argument moved |
| `_View` struct | any object | members readable | owners untouched | still owns it |

### Pointer / return

| Annotation | Implementation | Caller |
|---|---|---|
| *(none)* `T*` | returns non-null, pointee initialized | uses it as a view |
| `_Opt` | may return null | must check for null |
| `_Owner` | gives the object away | must free or move it |
| `_Uninitialized` pointee | returns raw memory | writes before reading |
| `_Clear` pointee | returns zeroed memory | nothing extra |

### Object state

| How it was created | State |
|---|---|
| `struct X x;` / `malloc` | uninitialized: only `_Out` may receive it |
| `= {}` / `calloc` | `_Opt` object: non-null members are zero; `_Dtor` accepts it |
| after all members assigned | complete: meets the default contract |
