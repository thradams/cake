# Cake Static Analysis — Ownership & Nullable Contracts

*Last Updated: May 2026*

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

Cake understands the built-in semantics of `malloc()` and `calloc()`:

- **`malloc()`** — returns an uninitialized block; members must be assigned before use.
- **`calloc()`** — returns a zero-initialized block; non-nullable members start as null and must be assigned before passing to functions expecting non-nullable pointers.

<!-- runnable -->

```c
#pragma nullable enable

char * _Opt strdup(const char * src);  
void * _Opt malloc(unsigned int sz);

struct X {  char * text; };  

void f() {     
   struct X * _Opt pX = malloc(sizeof *pX);
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
void * _Opt calloc(unsigned int n, unsigned int sz);

struct X {  
    char * text; //non-nullable
};  

void f0(struct X* p) { }

void f() {     
   struct X * _Opt pX = calloc(1, sizeof * pX);
   if (pX)
   {
      f0(pX); //warning 33: non-nullable pointer 'pX.text' may be null          
   }
}
```



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

FILE * _Owner _Opt fopen( const char *filename, const char *mode );
void fclose(FILE * _Owner p); /*p is not nullable*/

int main() {
    FILE * _Owner _Opt f = fopen("file.txt", "r");
    if (f)
        fclose(f);  // ownership is transferred to fclose
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


You can build a `delete` function on top of `x_destroy`:

```c
void x_delete(_Opt struct X * _Owner _Opt p) {
    if (p) {
        x_destroy(p);  // moves *p's contents
        free(p);       // releases storage
    }
}
```





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



### Inspecting State with `assert_state` and `static_debug`

Use these built-in declarations to inspect or assert variable states during development:

```c
#pragma safety enable

int main() {
    int a;
    assert_state(a, "uninitialized");  // asserts the current state at compile time
    static_debug(a);                   // prints state to compiler output
}
```

String literals are used because states like `"uninitialized"` and `"moved"` have no runtime representation.


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
    assert_state(p, "not-null");     // ok inside the check
}
assert_state(p, "null | not-null"); // outside: could be either
free(p);
```



### Overriding State with `override_state`

Sometimes the analyzer cannot determine the correct state — for example, when a non-pointer owner (like a socket) uses a sentinel value such as `-1` to represent "no resource". Use `override_state()` to assert the correct state explicitly:

```c
_Owner int server_socket = socket(AF_INET, SOCK_STREAM, 0);
if (server_socket < 0) {
    override_state(server_socket, "null");  // no resource; safe to return
    return;
}
/* ... */
close(server_socket);
```

`override_state` is also needed with `realloc()`, whose ownership semantics depend on the return value:

```c
void * _Owner _Opt p2 = realloc(p, 2);
if (p2 != 0) {
    override_state(p, "moved");  // realloc moved p on success
    p = p2;
}
free(p);
```

> **Note** Note: `override_state` is a dangerous function and should be avoided. Before using it, consider alternatives such as warning suppression or assert.



## Chapter 5: Known Limitations

Cake's flow analysis is not intra-procedural ; it does not trace relationships across function call 
boundaries or between separate branches that share the same underlying condition. 
This leads to a small number of false positives in certain patterns.

### Correlated Conditions

If the same condition governs two separate branches, the analyzer may warn inside the second branch because it has forgotten the correlation:

```c
int f(int c) {
    int i = 0;
    int * _Opt p = 0;
    if (c > 2) p = &i;
    if (c > 2) i = *p;  // warning: possible null dereference
}
```

> **Workaround:** Combine the two `if` blocks into one, or add `assert(p != 0)` before the dereference.



### State Relationships After Branching

When an owner is freed in one branch and set to null, the analyzer may still report it as possibly having a lifetime-ended state in a later check:


<!-- runnable -->

```c
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int * _Owner _Opt f(int c) {
    int * _Owner _Opt p = malloc(sizeof *p);
    try {
        if (c) throw; 
    }
    catch {
         free(p); 
         p = nullptr; 
    }
    return p;  // warning: possible lifetime-ended 'p'
}
```


These are implementation constraints, not flaws in the ownership model itself.



## Chapter 6: Code Transition Strategy

Adopting Cake's static analysis in an existing codebase does not require a big-bang migration. The recommended approach is incremental:

1. **Create `safe.h`** — define all Cake extensions (`_Owner`, `_Opt`, `_View`, `_Dtor`, `_Ctor`, `assert_state`, `override_state`, `static_debug`) as empty macros. This lets your code compile cleanly with a standard C compiler before you begin annotating.

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



### Pragmas

`#pragma nullable enable` / `#pragma nullable disable` — toggles non-nullable enforcement. When enabled, any pointer without `_Opt` is treated as non-nullable.

`#pragma ownership enable` — enables owner reference lifetime checks.

`#pragma safety enable` — enables both nullable and ownership checks at once. Equivalent to combining both pragmas above.

`#pragma flow enable` — enables flow analysis without enabling nullable or ownership rules.



### Built-in Intrinsics

`assert_state(var, "state")` — asserts that `var` is currently in the given state. Useful for verifying your understanding of what the analyzer tracks at a given point.

`static_debug(var)` — prints `var`'s current state to the compiler output. Use this to explore what the analyzer knows about a variable.

`override_state(var, "state")` — overrides `var`'s state in the analyzer. Use this when the analyzer cannot infer the correct state on its own, such as after `realloc()` or when working with non-pointer owners.


## References

- C# Nullable References: https://learn.microsoft.com/en-us/dotnet/csharp/nullable-references
- C# Nullable Migration Strategies: https://learn.microsoft.com/en-us/dotnet/csharp/nullable-migration-strategies
- TypeScript Type Predicates: https://www.typescriptlang.org/docs/handbook/advanced-types.html#using-type-predicates
- Microsoft SAL: https://learn.microsoft.com/en-us/cpp/code-quality/understanding-sal
- C# `out` parameter: https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/out
- C# Nullable Analysis Attributes: https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/attributes/nullable-analysis