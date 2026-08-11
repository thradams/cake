# Cake Static Analysis — Ownership & Nullable Contracts

*Last Updated: August 2026*

A hands-on guide to Cake's ownership and nullable pointer annotations - with working examples, 
enforced rules, and an incremental migration strategy for existing codebases.



## Introduction

Cake provides a set of contract annotations recognized 
by its static analyzer. With ownership annotations, you can achieve the 
same guarantees as C++ RAII and often stronger ones. 
Cake also introduces nullable pointer annotations, making it explicit 
when a pointer may be null and preventing mistakes like accidentally 
dereferencing a null pointer.

This manual walks you through each concept with working code examples, 
explains the rules enforced by the analyzer, and shows you how to adopt 
these features incrementally in an existing codebase.



## Chapter 1: Nullable Pointers

### The `_Opt` pointer annotation

The `_Opt` pointer annotation explicitly marks a pointer as nullable. 
The absence of `_Opt` means the pointer is non-nullable. 
The annotation is placed after the `*`.

For example, the following declaration says that `strdup()` accepts a non-nullable pointer and returns a nullable one:

```c
char * _Opt strdup(const char * src);
```

A pointer without `_Opt` is always assumed to be non-nullable.



### Enabling Nullable Rules: `#pragma nullable`

Because existing C code was not written with nullability in mind, Cake provides a pragma to control when the new rules apply:

```c
// new rules apply: absence of _Opt = non-nullable
#pragma nullable enable   

// Unannotated source: all unannotated pointers are nullable
#pragma nullable disable  
```

This lets you migrate code incrementally enabling the rules file-by-file or region-by-region. 
Only static analysis behavior changes; the runtime behavior of your program is unaffected.



### Example 1: Assigning Nullable to a Non-Nullable Pointer

Once nullable rules are enabled, assigning `nullptr` to an unqualified pointer generates a warning:

<!-- runnable -->

```c
#pragma nullable enable

int main() {
  int * p = nullptr;  /* warning: p is non-nullable */
}
```



### Example 2: Converting Non-Nullable to Nullable

Assigning a non-nullable pointer to a nullable variable is always safe and explicitly allowed:

<!-- runnable -->

```c
#pragma nullable enable

char * get_name();   // returns non-nullable

int main() {
  char * _Opt s = get_name();  // ok: widening to nullable
}
```



### Example 3: Nullable Pointer to a Non-Nullable Parameter

Narrowing from nullable to non-nullable requires a null check. 
Without one, the analyzer warns you:

<!-- runnable -->

```c
#pragma nullable enable

char * _Opt strdup(const char * src);
void f(char *s);   // s is non-nullable

int main() {
  char * _Opt s1 = strdup("a");
  f(s1);  /* warning: s1 may be null */
}
```


To fix this, add a null check before the call:

<!-- runnable -->

```c
#pragma nullable enable

char * _Opt strdup(const char * src);
void f(char *s);   // s is non-nullable

int main() {
  char * _Opt s1 = strdup("a");
  if (s1)
     f(s1);  // ok: flow analysis confirms s1 is not null here
}

```

Cake uses flow analysis to track possible nullability through branches.
Once you check a pointer, the analyzer knows it is non-null inside the guarded block.



### Helping the Analyzer with `assert()`

Because Cake's analysis is not inter-procedural, 
it cannot infer postconditions from called functions. 

When the analyzer cannot determine a pointer's state on its own, you can hint with `assert()`:

<!-- runnable -->

```c
#pragma safety enable

struct X { int * _Opt data; };

bool is_empty(struct X * p) {
    return p->data == nullptr;
}

void f(struct X * p) {
   if (!is_empty(p)) {
      assert(p->data != nullptr);  // hint to the analyzer
      *p->data = 1;               // ok
   }
}
```


From a flow analysis perspective, `assert(expr)` is equivalent to `if (!(expr)) exit(1);`.

> **Note:** Be cautious: the problem with this approach is the separation between where the postcondition is established and where the assert is placed. If `is_empty` changes, it may invalidate the caller's assert.

> **Note:** A contract-based approach (postconditions declared alongside the function) is under development and will eventually replace the need for remote `assert()` hints.



### Non-Nullable Initialization

Non-nullable pointers can be initialized with `{}`, meaning they are set to zero; however, 
they are still in an invalid state despite having a value. 
This is very similar to being uninitialized. For instance:

<!-- runnable -->

```c
#pragma nullable enable

struct X { char * text; };

struct X f() {
    struct X x;
    return x;  // warning: returning uninitialized 'x.text'
}
```


Compare with:


<!-- runnable -->

```c
#pragma nullable enable

struct X { char * text; };

struct X f() {
    struct X x = {};
    return x;  // warning: returning possible null pointer 'x.text'
}
```


In both cases, the object is in an invalid state. In the first case, `x.text` is uninitialized (it has no defined value). 
In the second case, `x.text` is initialized to zero (null), which is a defined value but still invalid for a non-nullable pointer.

<!-- runnable -->

```c
#pragma nullable enable

char * _Opt strdup(const char * src);  

struct X { char * text; };  

void f() {  
   char * _Opt s = strdup("a");
   if (s == nullptr)
     return;
   struct X x;
   x.text = s; //ok
}
```



### Using `malloc()` and `calloc()` with Structs

Cake understands the built-in semantics of `malloc()` and `calloc()` through the
`_Uninitialized` and `_Clear` pointee annotations declared on their return types:

```c
void * _Owner _Opt _Uninitialized malloc(size_t size);
void * _Owner _Opt _Clear calloc(size_t nmemb, size_t size);
```

- **`_Uninitialized`** (`malloc()`) — the returned pointee is uninitialized; every member must be
  assigned before use, exactly like a local variable declared without an initializer.
- **`_Clear`** (`calloc()`) — the returned pointee is zero-initialized; non-nullable members start
  as null and must be assigned before passing to functions expecting non-nullable pointers.

<!-- runnable -->

```c
#pragma nullable enable

char * _Opt strdup(const char * src);  
void * _Owner _Opt _Uninitialized malloc(unsigned int sz);

struct X {  char * text; };  

void f() {     
   struct X * _Owner _Opt pX = malloc(sizeof *pX);
   if (pX)
   {
      char * _Opt s = strdup("a");
      if (s != nullptr){
        pX->text = s; //ok
      }     
   }
}
```

<!-- runnable -->

```c
#pragma safety enable  

char * _Opt strdup(const char * src);  
void * _Owner _Opt _Clear calloc(unsigned int n, unsigned int sz);
void free(void * _Owner _Opt p);

struct X {  
    char * text; //non-nullable
};  

void f0(struct X* p) { }

void f() {     
   struct X * _Owner _Opt pX = calloc(1, sizeof * pX);
   if (pX)
   {
      f0(pX); //warning 33: non-nullable pointer 'pX.text' may be null          
      free(pX);
   }
}
```



### Comparison with C# and TypeScript

C#'s nullable reference types and TypeScript's `strictNullChecks` solve the same basic problem as
`_Opt`: absence of an explicit nullable annotation means non-nullable, and the compiler narrows a
variable's nullability after a check. The core mechanics are close enough that migrating mental
models between them is straightforward.

The two ecosystems diverge on power in different directions.

Cake's flow analysis tracks correlated facts *within* a function that C#/TS's narrowing does not
attempt — for example, proving `a + b == 3 || a + b == 7` (never `5`) after
`if (c) { a = 1; b = 2; } else { a = 3; b = 4; }`, by tracking which branch each value's alternative
originated from. C#/TS narrow nullability per variable; they don't correlate arithmetic or
relational facts across variables through a branch join at all.

What C#/TS have that Cake's *nullability* analysis does not yet have is **interprocedural**
reasoning. `[NotNullWhen]`, `[MaybeNullWhen]`, and TypeScript's type predicates and assertion
functions let a callee declare a postcondition that the caller's analysis picks up automatically.
Cake's nullability analysis is not inter-procedural today — it cannot infer what a called function
does to a pointer's nullability, so the same effect currently requires a manual `assert()` hint at
the call site (see *Helping the Analyzer with `assert()`*, above), with the attendant risk that the
hint goes stale if the callee's behavior changes. A `[NotNullWhen]`-equivalent for nullability is
planned; see the contract-based approach mentioned under *Helping the Analyzer with `assert()`*.

Cake's *ownership* analysis, however, already has a working precedent for exactly this shape of
contract: `_Ctor` and `_Dtor` are postconditions declared on the function signature itself
(`_Ctor` — "every member will be initialized before I return"; `_Dtor` — "every `_Owner` member
will be released before I return"), checked against the callee's actual implementation, and
consumed automatically by every caller without a manual hint — the same shape as C#'s `out`
definite assignment (see *Comparison with C#'s `out` parameters*, Chapter 3). So the interprocedural
gap above is specific to nullability, not a limitation of Cake's flow analysis in general.



## Chapter 2: Object Lifetime and Ownership

Object lifetime as the portion of program execution during which storage 
is reserved for that object. Cake's ownership system gives you compile-time enforcement 
of these rules.

### Enabling Ownership Checks

```c
// enables lifetime checks
#pragma ownership enable  

// equivalent to: nullable enable + ownership enable
#pragma safety enable      
```

> **Note:** `_Owner` annotations are parsed even when ownership is disabled, but have no effect. Use `#pragma safety enable` as a shorthand for both features.



### Owner References and the `_Owner` annotation

An **owner reference** is an object that manages the lifetime of the thing it references. The most common form is an **owner pointer** — a pointer to a heap-allocated object that is responsible for freeing it.

Declare an owner pointer by adding `_Owner` after the `*`:

<!-- runnable -->

```c
#pragma safety enable

typedef int FILE;

FILE * _Owner _Opt fopen( const char *filename, const char *mode );
void fclose(FILE * _Owner p); /* p is not nullable */

int main() {
    FILE * _Owner _Opt f = fopen("file.txt", "r");
    if (f)
        fclose(f);  /* ownership is transferred to fclose */
}
```


If you comment out `fclose()`, the analyzer warns that `f` leaves scope without its ownership being transferred:

```c
} // warning: 'f' leaves scope with an owned resource
```



### The Two Core Ownership Rules

> **Rule:** An owner reference is always the unique owner of the referenced object. Copying an owner reference transfers ownership to the destination.

> **Rule:** Before the end of its lifetime, an owner reference must move ownership of the objects it owns.

Ownership transfer happens through assignment or by passing the owner as a function argument. Calling `fclose(f)` is exactly like assigning `f` to `fclose`'s parameter — the callee takes ownership:

```c
FILE * _Owner _Opt f2 = f;  // ownership transferred: f is now 'moved'
if (f2) fclose(f2);         // f2's ownership transferred to fclose
```



### Additional Owner Rules

> **Rule:** Owner objects cannot be discarded. Calling `fopen()` and ignoring the return value is an error.

```c
fopen("file.txt", "r");  // warning: discarding owner return value
```

> **Rule:** A non-owner object cannot be copied to an owner object.

> **Rule:** The null pointer constant can be used to initialize owner objects (it represents an empty owner).



### Non-Pointer Owner References

Ownership is not limited to pointers. Berkeley sockets, for example, use an integer file descriptor. You can mark any type as an owner:

```c
_Owner int server_socket = socket(AF_INET, SOCK_STREAM, 0);
/* ... */
close(server_socket);
```

> **Note:** The `_Owner` annotation belongs to the storage holding the reference, not to the pointed-to type. Its placement follows the same rules as `const`.



### View References

A **view reference** accesses an object without managing its lifetime. Regular (non-`_Owner`) 
pointers are view references by default. The `_View` annotation is used on struct types, 
not on pointer declarations.

> **Rule:** The lifetime of the referenced object must exceed the lifetime of the view reference.

The analyzer detects when a view pointer outlives what it points to:

<!-- runnable -->

```c
struct X { int i; };

int main()
{
    struct X * _Opt p = nullptr;

    {
        struct X x = {};
        p = &x;
    }

    p->i = 1;  // warning: 'x' lifetime ended
    static_debug(p);
}

```



### The `_View` annotation

The `_View` annotation enables assignment without ownership transfer. 
A `_View` object does not own the resource.

<!-- runnable -->

```c
#pragma safety enable

struct X { char * _Owner _Opt text; };

void f(_View struct X x) { /* read-only access, no ownership */ }
void free(void* _Owner _Opt p);

int main() {
    struct X x = {};
    f(x); /* not moved */
    
    _View struct X x2 = x; /* not moved */

    free(x.text);   /* x is the owner */
}

```






### Deleting Owner Pointers

Owner pointers take responsibility for both the pointed-to object and its storage. 
A common pattern is to implement a `delete` function that releases both:

<!-- runnable -->

```c
#pragma safety enable
#include <stdlib.h>

struct X { char * _Owner text; };

void x_delete(struct X * _Owner _Opt p) {
  if (p) {
    free(p->text);
    free(p); /* storage */
  }
}
```



> **Note:** `void * _Owner`  owns *only the storage*.

> **Note:** Converting `T * _Owner` to `void * _Owner` requires the pointed-to object to be empty first. This is how the analyzer distinguishes between moving the object and moving only the raw storage.



### Comparison with C++ RAII


RAII is a pattern, not a static check: a constructor/destructor pair runs automatically at 
scope exit, but nothing verifies that a raw resource outside such a wrapper was released. 
`T* p = new T;` with no matching delete compiles without complaint in C++. The guarantee only 
exists for types that actually implement it.

Cake's `_Owner` takes the opposite approach: it enforces the same discipline directly on ordinary
pointers and values, without requiring a wrapper type. Leaving scope with an unmoved `_Owner` 
is a compile-time warning, not a convention you have to trust. The fopen/fclose example earlier 
in this chapter is exactly the RAII pattern, but checked by the analyzer instead of relying 
on a class to run the cleanup.

The two approaches provide different guarantees. RAII executes the destructor automatically 
at scope exit. Cake's _Ctor/_Dtor are contracts that the analyzer verifies were fulfilled, 
but no code is generated to call them automatically. As a result, there are code paths that 
RAII would still clean up automatically, whereas Cake relies on the analyzer being able to 
verify that the required cleanup was written.

Cake also proves the ownership rules statically, with no runtime overhead. `std::unique_ptr`,
which is the closest analog to `_Owner` (single, move-only ownership), still performs a runtime
check in its destructor. After a move, its internal pointer is set to null so that `delete ptr;`
becomes a no-op. 
Every destructor therefore executes a null check, whether or not the object was ever moved from.
Cake proves the equivalent fact "this owner still holds something to release" ahead of time by tracking
ownership and moved/null state through flow analysis, so no unconditional runtime check needs to be emitted.
When the analyzer cannot prove that an owner is null, an explicit `if (ptr)` provides the missing proof,
allowing the cleanup to be verified while keeping the common case free of unnecessary runtime checks.



## Chapter 3: Parameter annotations

### The `_Dtor` parameter annotation

`_Dtor` tells the analyzer that the function will move out all owner contents of 
the pointed-to struct, leaving it uninitialized. The compiler also verifies that the 
implementation fulfills this contract.

<!-- runnable -->

```c
#pragma safety enable
#include <stdlib.h>

struct X { char * _Owner _Opt text; };

void x_destroy(_Dtor struct X * x) {
    free(x->text);  // moves x->text; contract fulfilled
}

int main() {
    struct X x = {};
    /* ... */
    x_destroy(&x);  // contents of x are now moved
}
```

> **Note:** `_Dtor` is implicitly `_Opt` on every member it reaches — the pointee is only
> guaranteed to be *partially* constructed, so each member is treated as possibly-null
> regardless of its own declared nullability. This lets `_Dtor` accept an object that failed
> halfway through initialization. It does not excuse the callee from releasing a member that
> does turn out to be non-null: the caller is excused from that release (its ownership ends at
> the call), so the callee must actually do it, typically behind an `if (x->member) ...` guard.


You can build a `delete` function on top of `x_destroy`:

```c
void x_delete(_Opt struct X * _Owner _Opt p) {
    if (p) {
        x_destroy(p);  // moves *p's contents
        free(p);       // releases storage
    }
}
```





### The `_Clear` parameter annotation

`_Clear` tells the analyzer that the function will zero out every member of the pointed-to
struct before returning. The compiler verifies that the implementation fulfills this contract —
each member must be provably zero at every exit point:

<!-- runnable -->

```c
#pragma safety enable

struct X { int a; int b; };

void zero_out(_Clear struct X* p) {
    p->a = 0;
    p->b = 0;  // both members must be zero at exit
}
```

Omitting a member (or leaving it possibly non-zero) is a warning:

```c
void zero_out(_Clear struct X* p) {
    p->a = 0;
    // p->b never touched
} // warning: _Clear parameter 'p' pointee (.b) is not zero at exit
```

`_Clear` follows the same placement rules as `_Ctor`/`_Dtor`: it annotates the pointee, not the
pointer, and the pointee cannot also be `const` (the callee has to write it).



### The `_Ctor` parameter annotation

`_Ctor` is the inverse of `_Dtor`. It tells the analyzer that the 
function expects an **uninitialized** object as input and initializes it 
on return. This is the pattern for init-style functions.

<!-- runnable -->

```c
#pragma safety enable
#include <string.h>

struct X { char * _Owner _Opt text; };

int init(_Ctor struct X *p, const char * text) {
    p->text = strdup(text);  // safe: p->text is uninitialized
}

int main() {
    struct X x;
    init(&x, "hello");  // x is fully initialized after this
    free(x.text);
}
```


Contrast this with a setter, which operates on an already-initialized object and
must free the old value first:

<!-- runnable -->

```c
#pragma safety enable
#include <string.h>

struct X { char * _Owner _Opt text; };

int set(struct X *p, const char * text) {
    // old value must be released
    //free(p->text);
    p->text = strdup(text);
}
```

#### Comparison with C#'s `out` parameters

`_Ctor` looks like C#'s `out`: both let the callee receive storage the caller has not initialized,
on the understanding that the callee will initialize it. C#'s `out` is backed by **definite
assignment** — the compiler rejects any code path in the callee that returns without assigning the
`out` parameter — and `_Ctor` is checked the same way, against the implementation, not just assumed:

<!-- runnable -->

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* src);

struct X { char* _Owner _Opt text; };

void x_init_forgot(_Ctor struct X* p) {
    /* p->text is never written */
} // warning: _Ctor parameter 'p' pointee (.text) is possibly not initialized at exit
```

Unlike `_Dtor` — which only obligates `_Owner` members, since a plain scalar has no lifetime to end
— `_Ctor` covers every member of the pointee, the same breadth as `out`. Leaving a non-`_Owner`
field untouched is just as much a violation as leaving an `_Owner` one uninitialized, because the
caller is trusting that the whole object is now well-formed, not only its owned resources:

```c
#pragma safety enable

char* _Owner _Opt strdup(const char* src);

struct X { char* _Owner _Opt text; int i; };

void x_init(_Ctor struct X* p) {
    p->text = strdup("hello");
    p->i = 0;
} // ok: every member initialized

void x_init_partial(_Ctor struct X* p) {
    p->text = strdup("hello");
    /* p->i never touched */
} // warning: _Ctor parameter 'p' pointee (.i) is possibly not initialized at exit
```


### Type Annotations in Arrays parameters

In C, array types in arguments are pointers. This characteristics is preserved.

To use the owner annotation in an array we do. (Just like const)

```c
#pragma safety enable

void free(void * _Owner _Opt p);

void f(int a[_Owner])
{
  free(a);
}

```

But I think this is quite uncommon.

## Chapter 4: Flow Analysis and Object States

Flow analysis is the foundation of Cake's nullable and ownership checks. 
It tracks the possible states of every variable at every point in your program.

### Enabling Flow Analysis

```c
#pragma flow enable    // standalone
#pragma safety enable  // enables flow automatically
// or compile with: -fanalyzer
```



### The State Model

The analyzer tracks the following states for each variable:

`uninitialized` — the variable has been declared but not yet assigned a value.

`moved` — ownership was transferred to another variable or passed into a function call.

`null` — the pointer is null, or the owner holds no resource.

`not-null` — the pointer is known to reference a live object.

`zero` — the value of a non-pointer object is zero. This is distinct from `null` because, for non-pointer owners like sockets, zero does not necessarily mean "no resource".

`not-zero` — the value of a non-pointer object is non-zero.

`lifetime-ended` — the object the pointer was referencing has gone out of scope.

> **Note:** `uninitialized`, `moved`, and `lifetime-ended` are imaginary states: they exist in flow
> analysis but not at runtime, and have no corresponding value in memory. The other states (`null`,
> `not-null`, `zero`, `not-zero`) describe something actually stored in the variable. But nothing
> at runtime is "uninitialized," "moved," or "lifetime-ended" — the bits sitting in memory are just
> bits (a moved-from or lifetime-ended pointer can still hold a perfectly valid-looking address).
> These states exist only so the analyzer can track *what it knows*: that no meaningful value has
> been given yet, that the variable is no longer accountable for the value it held, or that the
> object it once pointed to is no longer around.



### Inspecting State with `static_debug`

Use this built-in declaration to inspect a variable's tracked state during development:

```c
#pragma safety enable

int main() {
    int a;
    static_debug(a);   // prints state to compiler output
}
```



### Proving Facts with `compile_assert`

`compile_assert(expr)` asks the analyzer to prove `expr` using only what it already knows at that
point — across every value alternative the flow tracker has recorded, not just one possible path.
If it can't prove `expr` on every alternative, it warns:

```c
#pragma safety enable

void f(int c) {
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    compile_assert(a + b == 3 || a + b == 7);  // ok: true on every path
    compile_assert(a + b == 3);                // warning: false on the c==0 path
}
```

Unlike `assert()` and `_Assert()`, `compile_assert` is purely a compile-time check:

- **It generates no code and has no runtime effect** — the condition is never evaluated. It exists
  only for the analyzer.
- **It does not narrow later state.** `assert()`/`_Assert()` discard the branches where the
  condition is false, so the analyzer treats it as proven from that point on; `compile_assert` does
  not — it only reports whether the fact is *already* provable, and leaves the tracked state
  untouched either way:

```c
#pragma safety enable

void f(int* _Opt p) {
    compile_assert(p != 0);  // warning: not provable here (p could be null)
    *p = 1;                  // warning: possible null pointer dereference (unaffected by the assert above)
}
```

Because of this, `compile_assert` is mainly a development and regression-testing tool: use it to
pin down exactly what the analyzer can currently prove at a given point, and to catch the analyzer
regressing on a previously-provable fact. Prefer `assert()`/`_Assert()` in real code where
you actually want to establish a fact for the analysis (and the runtime) going forward.



### Uninitialized State

Reading an uninitialized variable triggers a warning. This also applies after a move:

```c
int * _Owner p = f();
free(p);   // p is now uninitialized (moved into free)
free(p);   // warning: p is uninitialized
```



### Moved State

When you assign an owner to another owner in local scope, the source enters the moved state. The analyzer knows the pointed-to object is still valid but the source no longer owns it:

```c
int * _Owner p = f();
int * _Owner _Opt p2 = 0;
p2 = p;       // p is now 'moved'
free(p);      // warning: p was moved
free(p2);     // ok
```



### Null and Not-Null States

After a null check, the analyzer refines the state inside each branch:

```c
void * _Owner _Opt p = malloc(1);
if (p) {
    compile_assert(p != 0);  // ok: provable inside the check
}
free(p);
```



## Chapter 5: Known Limitations

### Backward `goto` Is Not Analyzed as a Loop

Unlike `while`/`for`, which run a two-pass analysis to approximate repeated execution, a label is
visited only once, in top-to-bottom order. A backward `goto` (jumping to a label above it) does not
re-trigger analysis of the label's body, so the analyzer under-approximates what the jump can do:

```c
#pragma safety enable

void f(int n) {
    int a = 0;
L:
    a = a + 1;
    if (n > 0) {
        n = n - 1;
        goto L;
    }
    // Expected (sound): a is an unbounded value, since the loop can run any
    // number of times.
    // Actual (today): a is seen as exactly 1, as if the backward jump had
    // no effect on the analysis.
    compile_assert(a == 1);
}
```

> **Workaround:** Prefer `while`/`for` over backward `goto` where possible; those constructs get the
> full two-pass loop analysis.

### Pointer Equality Between Distinct Pointers

Two unrelated pointers may still compare equal at runtime, but the analyzer currently treats a
concrete (synthetic) pointer value as foldable, so `p == q` for two distinct pointees folds to
always-false and the true branch is (incorrectly) treated as dead:

```c
#pragma safety enable

void ptr_eq(int* _Opt p, int* _Opt q) {
    if (p == q) {
        // This branch is reachable at runtime and must still be analyzed,
        // but flow3 currently folds the comparison to always-false here.
        if (p) {
            compile_assert(p != 0);
        }
    }
}
```

> **Note:** Comparisons against the literal `0`/`nullptr` are unaffected — only two non-constant
> pointer operands trigger this.

These are implementation constraints, not flaws in the ownership model itself.



## Chapter 6: Code Transition Strategy

Adopting Cake's static analysis in an existing codebase does not require a big-bang migration. The recommended approach is incremental:

1. **Create `safe.h`** — define all Cake extensions (`_Owner`, `_Opt`, `_View`, `_Dtor`, `_Ctor`, `compile_assert`, `static_debug`) as empty macros. This lets your code compile cleanly with a standard C compiler before you begin annotating.

2. **Enable nullable rules one file at a time** — add `#pragma nullable enable` to one translation unit, fix its warnings, then move to the next.

3. **Enable ownership rules** — once nullable warnings are clean in a file, add `#pragma ownership enable` (or `#pragma safety enable`).

4. **Annotate signatures progressively** — add `_Owner`, `_Opt`, `_Ctor`, and `_Dtor` annotations as you work through each file. The pragma-controlled rollout ensures you always have a compiling codebase.



## Quick Reference

### Type Annotations

`_Opt` — the pointer may be null (nullable). Without this type annotation, a pointer is treated as non-nullable when nullable rules are enabled.

`_Owner` — the reference manages the lifetime of the object it references. Ownership is transferred on assignment or when passed to a function.

`_View` on struct — strips `_Owner` from all members for the duration of that variable's scope. Used to pass an owner struct without transferring ownership.

`_Ctor` — the parameter must be uninitialized on entry; the function is responsible for initializing it before returning.

`_Dtor` — the parameter must be fully initialized on entry; the function is responsible for moving out all owner contents before returning.

`_Clear` — the function is responsible for setting every member of the pointee to zero before returning. Also used on a function's return type (e.g. `calloc()`) to describe an already zero-initialized pointee.

`_Uninitialized` — marks the pointee as uninitialized. Used on a function's return type (e.g. `malloc()`) to describe a freshly allocated, uninitialized pointee.



### Pragmas

`#pragma nullable enable` / `#pragma nullable disable` — toggles non-nullable enforcement. When enabled, any pointer without `_Opt` is treated as non-nullable.

`#pragma ownership enable` — enables owner reference lifetime checks.

`#pragma safety enable` — enables both nullable and ownership checks at once. Equivalent to combining both pragmas above.

`#pragma flow enable` — enables flow analysis without enabling nullable or ownership rules.



### Built-in Intrinsics

`compile_assert(expr)` — asks the analyzer to prove `expr` from what it currently knows; warns if it can't. Purely compile-time — no runtime effect, and it does not narrow later state.

`static_debug(var)` — prints `var`'s current state to the compiler output. Use this to explore what the analyzer knows about a variable.


## References

- C# Nullable References: https://learn.microsoft.com/en-us/dotnet/csharp/nullable-references
- C# Nullable Migration Strategies: https://learn.microsoft.com/en-us/dotnet/csharp/nullable-migration-strategies
- TypeScript Type Predicates: https://www.typescriptlang.org/docs/handbook/advanced-types.html#using-type-predicates
- Microsoft SAL: https://learn.microsoft.com/en-us/cpp/code-quality/understanding-sal
- C# `out` parameter: https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/out
- C# Nullable Analysis Attributes: https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/attributes/nullable-analysis