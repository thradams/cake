# Cake Static Analysis — Ownership & Nullable Pointers: Programmer's Manual

*Last Updated: December 2025*

---

## Introduction

Cake provides a set of annotations and extended qualifiers recognized 
by its static analyzer. With ownership qualifiers, you can achieve the 
same guarantees as C++ RAII and often stronger ones. 
Cake also introduces nullable pointer semantics, making it explicit 
when a pointer may be null and preventing mistakes like accidentally 
dereferencing a null pointer.

This manual walks you through each concept with working code examples, 
explains the rules enforced by the analyzer, and shows you how to adopt 
these features incrementally in an existing codebase.

---

## Chapter 1: Nullable Pointers

### The `_Opt` Qualifier

The qualifier `_Opt` explicitly marks a pointer as nullable. 
The absence of `_Opt` means the pointer is non-nullable. 
Like `const`, the qualifier is placed after the `*`.

For example, the following declaration says that `strdup()` accepts a non-nullable pointer and returns a nullable one:

```c
char * _Opt strdup(const char * src);
```

A pointer without `_Opt` is always assumed to be non-nullable when `#pragma nullable enable` is in effect.

---

### Enabling Nullable Rules: `#pragma nullable`

Because existing C code was not written with nullability in mind, Cake provides a pragma to control when the new rules apply:

```c
#pragma nullable enable   // new rules apply: absence of _Opt = non-nullable
#pragma nullable disable  // legacy mode: all unqualified pointers are nullable
```

This lets you migrate code incrementally — enabling the rules file-by-file or region-by-region. Only static analysis behavior changes; the runtime behavior of your program is unaffected.

---

### Example 1: Assigning null to a Non-Nullable Pointer

Once nullable rules are enabled, assigning `nullptr` to an unqualified pointer generates a warning:

```c
#pragma nullable enable

int main() {
  int * p = nullptr;  // warning: p is non-nullable
}
```

> **Note:** `p` has no `_Opt` qualifier, so the analyzer treats it as non-nullable. Assigning `nullptr` violates this contract.

---

### Example 2: Converting Non-Nullable to Nullable

Assigning a non-nullable pointer to a nullable variable is always safe and explicitly allowed:

```c
#pragma nullable enable

char * get_name();   // returns non-nullable

int main() {
  char * _Opt s = get_name();  // ok: widening to nullable
}
```

---

### Example 3: Passing a Nullable Pointer to a Non-Nullable Parameter

Narrowing from nullable to non-nullable requires a null check. Without one, the analyzer warns you:

```c
#pragma nullable enable

char * _Opt strdup(const char * src);
void f(char *s);   // s is non-nullable

int main() {
  char * _Opt s1 = strdup("a");
  f(s1);  // warning: s1 may be null
}
```

To fix this, add a null check before the call:

```c
  if (s1)
    f(s1);  // ok: flow analysis confirms s1 is not null here
```

Cake uses flow analysis to track possible nullability through branches. Once you check a pointer, the analyzer knows it is non-null inside the guarded block.

---

### Helping the Analyzer with `assert()`

Because Cake's analysis is not inter-procedural, it cannot infer postconditions from called functions. When the analyzer cannot determine a pointer's state on its own, you can hint with `assert()`:

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

> **Note:** A contract-based approach (postconditions declared alongside the function) is under development and will eventually replace the need for remote `assert()` hints.

---

### Non-Nullable Member Initialization

Non-nullable struct members follow similar rules to `const` members, but with one key difference: they can transition from uninitialized to non-null (whereas `const` cannot change after declaration).

The analyzer warns if you return or pass an object with an uninitialized non-nullable member:

```c
#pragma nullable enable

struct X { char * text; };

struct X f() {
   struct X x;
   return x;  // warning: returning uninitialized 'x.text'
}
```

Zero-initialization sets non-nullable members to null, which is also flagged:

```c
struct X f() {
   struct X x = {};
   return x;  // warning: returning possible null pointer 'x.text'
}
```

---

### Using `malloc()` and `calloc()` with Structs

Cake understands the built-in semantics of `malloc()` and `calloc()`:

- **`malloc()`** — returns an uninitialized block; members must be assigned before use.
- **`calloc()`** — returns a zero-initialized block; non-nullable members start as null and must be assigned before passing to functions expecting non-nullable pointers.

To work around the `calloc()` null-member issue, apply `_Opt` to the struct type, which makes all members nullable for that declaration:

```c
_Opt struct X * _Opt pX = calloc(1, sizeof *pX);
// all members of *pX are now treated as nullable
```

---

## Chapter 2: Object Lifetime and Ownership

The C standard defines object lifetime as the portion of program execution during which storage is reserved for that object. Accessing an object outside its lifetime is undefined behavior. Cake's ownership system gives you compile-time enforcement of these rules.

### Enabling Ownership Checks

```c
#pragma ownership enable   // enables lifetime checks
#pragma safety enable      // equivalent to: nullable enable + ownership enable
```

> **Note:** `_Owner` qualifiers are parsed even when ownership is disabled, but have no effect. Use `#pragma safety enable` as a shorthand for both features.

---

### Owner References and the `_Owner` Qualifier

An **owner reference** is an object that manages the lifetime of the thing it references. The most common form is an **owner pointer** — a pointer to a heap-allocated object that is responsible for freeing it.

Declare an owner pointer by adding `_Owner` after the `*`:

```c
#pragma safety enable
#include <stdio.h>

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

---

### The Two Core Ownership Rules

> **Rule:** An owner reference is always the unique owner of the referenced object. Copying an owner reference transfers ownership to the destination.

> **Rule:** Before the end of its lifetime, an owner reference must move ownership of the objects it owns.

Ownership transfer happens through assignment or by passing the owner as a function argument. Calling `fclose(f)` is exactly like assigning `f` to `fclose`'s parameter — the callee takes ownership:

```c
FILE * _Owner _Opt f2 = f;  // ownership transferred: f is now 'moved'
if (f2) fclose(f2);         // f2's ownership transferred to fclose
```

---

### Additional Owner Rules

> **Rule:** Owner objects cannot be discarded. Calling `fopen()` and ignoring the return value is an error.

```c
fopen("file.txt", "r");  // warning: discarding owner return value
```

> **Rule:** A non-owner object cannot be copied to an owner object.

> **Rule:** The null pointer constant can be used to initialize owner objects (it represents an empty owner).

---

### Non-Pointer Owner References

Ownership is not limited to pointers. Berkeley sockets, for example, use an integer file descriptor. You can mark any type as an owner:

```c
_Owner int server_socket = socket(AF_INET, SOCK_STREAM, 0);
/* ... */
close(server_socket);
```

> **Note:** The `_Owner` qualifier belongs to the storage holding the reference, not to the pointed-to type. Its placement follows the same rules as `const`.

---

### View References

A **view reference** accesses an object without managing its lifetime. Regular (non-`_Owner`) pointers are view references by default. The `_View` qualifier is used on struct types, not on pointer declarations.

> **Rule:** The lifetime of the referenced object must exceed the lifetime of the view reference.

The analyzer detects when a view pointer outlives what it points to:

```c
struct X * _Opt p = nullptr;
{
    struct X x = {};
    p = &x;
}
p->i = 1;  // warning: 'x' lifetime ended
```

---

### Using `_View` on Structs

When you need to pass an owner struct without transferring ownership, qualify the parameter with `_View`. This strips `_Owner` from all members for the scope of the parameter:

```c
struct X { char * _Owner _Opt text; };

void f(_View struct X x) { /* read-only access, no ownership */ }

int main() {
    struct X x = {};
    f(x);           // NOT moved — caller still owns x
    free(x.text);   // caller is still responsible
}
```

---

### Returning View Pointers

> **Rule:** You cannot return a view pointer to a local (automatic storage duration) variable.

```c
int * f() {
   int a = 1;
   return &a;  // ERROR: 'a' does not outlive the function
}
```

Returning pointers to static, thread-local, or heap-allocated objects is fine, as is returning a parameter pointer (the pointed-to object has a longer lifetime):

```c
static int a = 1;
int * f1() { return &a; }       // ok: static storage

int * f2(int *p) { return p; }  // ok: p outlives f2's frame
```

---

### Deleting Owner Pointers

Owner pointers take responsibility for both the pointed-to object and its storage. A common pattern is to implement a `delete` function that releases both:

```c
#pragma safety enable
#include <stdlib.h>

struct X { char * _Owner text; };

void x_delete(struct X * _Owner _Opt p) {
  if (p) {
    free(p->text);
    free(p);
  }
}
```

> **Note:** Converting `T * _Owner` to `void * _Owner` requires the pointed-to object to be empty first. This is how the analyzer distinguishes between moving the object and moving only the raw storage.

---

## Chapter 3: Destructors and Constructors

### The `[[dtor]]` Contract Attribute

`[[dtor]]` tells the analyzer that the function will move out all owner contents of the pointed-to struct, leaving it uninitialized. The compiler also verifies that the implementation fulfills this contract.

```c
#pragma safety enable
#include <stdlib.h>

struct X { char * _Owner _Opt text; };

void x_destroy([[dtor]] struct X * x) {
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

---

### The `[[ctor]]` Contract Attribute

`[[ctor]]` is the inverse of `[[dtor]]`. It tells the analyzer that the function expects an **uninitialized** object as input and initializes it on return. This is the pattern for init-style functions.

```c
int init([[ctor]] struct X *p, const char * text) {
    p->text = strdup(text);  // safe: p->text is uninitialized
}

int main() {
    struct X x;
    init(&x, "hello");  // x is fully initialized after this
    free(x.text);
}
```

Contrast this with a setter, which operates on an already-initialized object and must free the old value first:

```c
int set(struct X *p, const char * text) {
    free(p->text);          // release old value
    p->text = strdup(text);
}
```

---

### `[[dtor]]` vs `[[ctor]]` — Summary

Think of them as describing the state of the object before and after the call:

`[[ctor]]` — the input must be uninitialized or moved; the function's job is to fully initialize it.

`[[dtor]]` — the input must be fully initialized; the function's job is to move out all owner contents, leaving the object uninitialized.

---

## Chapter 4: Flow Analysis and Object States

Flow analysis is the foundation of Cake's nullable and ownership checks. It tracks the possible states of every variable at every point in your program.

### Enabling Flow Analysis

```c
#pragma flow enable    // standalone
#pragma safety enable  // enables flow automatically
// or compile with: -fanalyzer
```

---

### The State Model

The analyzer tracks the following states for each variable:

`uninitialized` — the variable has been declared but not yet assigned a value.

`moved` — ownership was transferred to another variable or passed into a function call.

`null` — the pointer is null, or the owner holds no resource.

`not-null` — the pointer is known to reference a live object.

`zero` — the value of a non-pointer object is zero. This is distinct from `null` because, for non-pointer owners like sockets, zero does not necessarily mean "no resource".

`not-zero` — the value of a non-pointer object is non-zero.

`lifetime-ended` — the object the pointer was referencing has gone out of scope.

---

### Inspecting State with `assert_state` and `static_debug`

Use these built-in declarations to inspect or assert variable states during development:

```c
#pragma safety enable

int main() {
    int a;
    assert_state(a, "uninitialized");  // asserts the current state
    static_debug(a);                   // prints state to compiler output
}
```

String literals are used because states like `"uninitialized"` and `"moved"` have no runtime representation.

---

### Uninitialized State

Reading an uninitialized variable triggers a warning. This also applies after a move:

```c
int * _Owner p = f();
free(p);   // p is now uninitialized (moved into free)
free(p);   // warning: p is uninitialized
```

---

### Moved State

When you assign an owner to another owner in local scope, the source enters the moved state. The analyzer knows the pointed-to object is still valid but the source no longer owns it:

```c
int * _Owner p = f();
int * _Owner _Opt p2 = 0;
p2 = p;       // p is now 'moved'
free(p);      // warning: p was moved
free(p2);     // ok
```

---

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

---

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

---

## Chapter 5: Known Limitations

Cake's flow analysis is intra-procedural — it does not trace relationships across function call boundaries or between separate branches that share the same underlying condition. This leads to a small number of false positives in certain patterns.

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

---

### State Relationships After Branching

When an owner is freed in one branch and set to null, the analyzer may still report it as possibly having a lifetime-ended state in a later check:

```c
int * _Owner _Opt f(int c) {
    int * _Owner _Opt p = malloc(sizeof *p);
    try { if (c) throw; }
    catch { free(p); p = nullptr; }
    return p;  // warning: possible lifetime-ended 'p'
}
```

These are implementation constraints, not flaws in the ownership model itself. Improvements are under active development.

---

## Chapter 6: Code Transition Strategy

Adopting Cake's static analysis in an existing codebase does not require a big-bang migration. The recommended approach is incremental:

1. **Create `safe.h`** — define all Cake extensions (`_Owner`, `_Opt`, `_View`, `[[dtor]]`, `[[ctor]]`, `assert_state`, `override_state`, `static_debug`) as empty macros. This lets your code compile cleanly with a standard C compiler before you begin annotating.

2. **Enable nullable rules one file at a time** — add `#pragma nullable enable` to one translation unit, fix its warnings, then move to the next.

3. **Enable ownership rules** — once nullable warnings are clean in a file, add `#pragma ownership enable` (or `#pragma safety enable`).

4. **Annotate signatures progressively** — add `_Owner`, `_Opt`, `[[ctor]]`, and `[[dtor]]` as you work through each file. The pragma-controlled rollout ensures you always have a compiling codebase.

---

## Quick Reference

### Qualifiers

`_Opt` — the pointer may be null (nullable). Without this qualifier, a pointer is treated as non-nullable when nullable rules are enabled.

`_Owner` — the reference manages the lifetime of the object it references. Ownership is transferred on assignment or when passed to a function.

`_View` on struct — strips `_Owner` from all members for the duration of that variable's scope. Used to pass an owner struct without transferring ownership.

`[[ctor]]` — the parameter must be uninitialized on entry; the function is responsible for initializing it before returning.

`[[dtor]]` — the parameter must be fully initialized on entry; the function is responsible for moving out all owner contents before returning.

---

### Pragmas

`#pragma nullable enable` / `#pragma nullable disable` — toggles non-nullable enforcement. When enabled, any pointer without `_Opt` is treated as non-nullable.

`#pragma ownership enable` — enables owner reference lifetime checks.

`#pragma safety enable` — enables both nullable and ownership checks at once. Equivalent to combining both pragmas above.

`#pragma flow enable` — enables flow analysis without enabling nullable or ownership rules.

---

### Built-in Intrinsics

`assert_state(var, "state")` — asserts that `var` is currently in the given state. Useful for verifying your understanding of what the analyzer tracks at a given point.

`static_debug(var)` — prints `var`'s current state to the compiler output. Use this to explore what the analyzer knows about a variable.

`override_state(var, "state")` — overrides `var`'s state in the analyzer. Use this when the analyzer cannot infer the correct state on its own, such as after `realloc()` or when working with non-pointer owners.

---

## References

- C# Nullable References: https://learn.microsoft.com/en-us/dotnet/csharp/nullable-references
- C# Nullable Migration Strategies: https://learn.microsoft.com/en-us/dotnet/csharp/nullable-migration-strategies
- TypeScript Type Predicates: https://www.typescriptlang.org/docs/handbook/advanced-types.html#using-type-predicates
- Microsoft SAL: https://learn.microsoft.com/en-us/cpp/code-quality/understanding-sal
- C# `out` parameter: https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/out
- C# Nullable Analysis Attributes: https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/attributes/nullable-analysis