# Cake Static Analysis Manual

Ownership, lifetime, and null-safety checks.

Two compiler options control which checks are active:

- **`nullable_enabled`** — null-safety checks on pointer types
- **`ownership_enabled`** — ownership and lifetime checks

---

## 1. Introduction

### 1.1 Object States

Every tracked object is in exactly one of the following states at any program point:

| State | Meaning |
|---|---|
| `UNINITIALIZED` | Object has not been given a value. Reading it is invalid. |
| `ANY` | Object holds some valid value; the exact value is unknown. |
| `== V` | Object is known equal to the concrete value V. |
| `!= V` | Object is known not equal to the concrete value V. |
| `moved` | Ownership was transferred away. Object must not be used again. |
| `expired` | Lifetime ended (e.g. passed to a destructor). Object must not be used. |

### 1.2 Qualifier Reference

| Qualifier | Meaning |
|---|---|
| `_Owner` | Object owns a resource; must be explicitly released before going out of scope. |
| `_Opt` | Pointer may be null. Without `_Opt` a pointer is assumed non-null. |
| `_Ctor` | Constructor context: pointed object starts uninitialized; must be initialized on exit. |
| `_Dtor` | Destructor context: pointed object must be uninitialized/moved/expired on exit. |
| `_View` | Non-owning reference. No ownership or exit checks are applied. |

---

## 2. Function Parameters

At function entry the analyser seeds the flow map with an initial state for every parameter.

### 2.1 Null State of Pointer Parameters

With `nullable_enabled` on:

- `_Opt` pointer — may be null or may point to a valid arena object.
- Non-`_Opt` pointer — assumed non-null (`!= 0`) at entry.

With `nullable_enabled` off:

- Any pointer (with or without `_Opt`) may be null or point to a valid arena object.

### 2.2 Initialization State of the Pointed Object

- Pointer to `_Ctor` type — the pointed object starts **UNINITIALIZED**.
- Any other pointer — the pointed object starts in the **ANY** state.

### 2.3 Exit Requirements for `_Ctor` and `_Dtor` Parameters

Before the function returns:

- Pointer to `_Ctor` — the pointed object **must be initialized**.
- Pointer to `_Dtor` — the pointed object **must be uninitialized, moved, or expired**.

```c
// Constructor: pointed object starts uninitialized, must be initialized at exit
void string_init(struct string * _Ctor p)
{
    p->data = malloc(64);
    p->len  = 0;
    // OK: both members are initialized before return
}

// Destructor: pointed object must be fully released at exit
void string_destroy(struct string * _Dtor p)
{
    free(p->data);  // p->data is now expired
    p->len = 0;
    // OK: owner member released, scalar zeroed
}
```

### 2.4 Exit Requirements for `_Owner` Parameters  *(ownership_enabled)*

**Pointer `_Owner` parameter** — for each possible pointer value at exit:

- Known non-null: the pointed object must be **moved, expired, or uninitialized**.
- Known null: no check needed.
- Unknown at compile time: a conservative resource-leak warning is emitted.

**Non-pointer `_Owner` parameter:**

- Struct with owner members: each owner member must be expired, uninitialized, or moved.
- Scalar: the object itself must be expired, uninitialized, or moved.

**`_View` parameter:** carries no ownership; no exit check is performed.

---

## 3. Return Values

### 3.1 Null Safety  *(nullable_enabled)*

With `nullable_enabled` on:

- Non-`_Opt` pointer return type — returned value must not be null.
- `_Opt` pointer return type — null is allowed; but the returned pointer must not be moved or expired.

With `nullable_enabled` off: no null constraint on pointer return values.

### 3.2 Initialization State

The returned object must not be in the `uninitialized`, `expired`, or `moved` state. The check is
unconditional — there is no context in which returning an uninitialized value is silently allowed.

```c
// INVALID — returning uninitialized object
int get_value(void)
{
    int x;
    return x;   // diagnostic: object is uninitialized
}

// VALID
int get_value(void)
{
    int x = 42;
    return x;
}
```

### 3.3 Ownership Transfer  *(ownership_enabled)*

- Returning an `_Owner` object or pointer transfers ownership to the caller.
- After the return the source object is considered **moved**.
- Returning an `_Owner` value into a non-`_Owner` return type is **invalid** (resource leak).

```c
// VALID — ownership transfer on return
struct string * _Owner make_string(void)
{
    struct string * _Owner p = malloc(sizeof *p);
    string_init(p);
    return p;   // ownership moves to caller
}

// INVALID — _Owner returned as non-_Owner
struct string * make_string(void)   // missing _Owner
{
    struct string * _Owner p = malloc(sizeof *p);
    return p;   // diagnostic: _Owner assigned to non-_Owner (resource leak)
}
```

---

## 4. Assignment

### 4.1 Source Object Validity

The source (right-hand side) of an assignment must never be **uninitialized**, **expired**, or
**moved**. This rule is unconditional.

```c
// INVALID — assigning a moved object
struct string * _Owner a = make_string();
struct string * _Owner b = a;   // a is now moved
struct string * _Owner c = a;   // diagnostic: object is moved
```

### 4.2 Destination State  *(ownership_enabled)*

When assigning to an `_Owner` destination, the destination must already be **expired, moved, or
uninitialized**. Overwriting a live owner leaks its resource.

```c
// INVALID — overwriting a live _Owner
struct string * _Owner p = make_string();
p = make_string();   // diagnostic: destination _Owner is not null/uninitialized/moved

// VALID — destination is properly released first
struct string * _Owner p = make_string();
string_destroy(p);
free(p);
p = NULL;
p = make_string();   // OK: destination was null
```

### 4.3 Null Safety  *(nullable_enabled)*

- Assigning null to a non-`_Opt` pointer is **invalid**.
- Assigning a possibly-null pointer to a non-`_Opt` destination is **invalid**.

```c
// INVALID
struct string * p;
p = NULL;   // diagnostic: assigning null to non-_Opt pointer

// VALID
struct string * _Opt p;
p = NULL;   // OK: _Opt allows null
```

### 4.4 Ownership Move  *(ownership_enabled)*

- Assigning an `_Owner` object performs a **move**: ownership is transferred to the destination.
- After assignment, the source becomes **moved**; it must not be used again.
- A moved object cannot be moved again.
- Copying an `_Owner` object without moving is **invalid**.

---

## 5. Initialization (Declarations)

### 5.1 Initial Object State

- Object declared with `_Ctor` — starts **UNINITIALIZED**.
- All other objects — start in the **ANY** state unless a more specific state can be inferred
  from the initializer.

### 5.2 Source Validity

The initializer expression must not be **uninitialized, expired, or moved**. This rule is
identical to §3 (return) and §4 (assignment) — there is no exception for declarations.

```c
// INVALID — initializing from a moved object
struct string * _Owner a = make_string();
struct string * _Owner b = a;   // a is moved
struct string * _Owner c = a;   // diagnostic: object is moved
```

### 5.3 Null Safety  *(nullable_enabled)*

- Initializing a non-`_Opt` pointer with null is **invalid**.
- Initializing an `_Opt` pointer with null is valid.

```c
// INVALID
struct string * p = NULL;   // diagnostic: non-_Opt pointer initialized with null

// VALID
struct string * _Opt p = NULL;
```

### 5.4 Ownership Move  *(ownership_enabled)*

- Initializing an `_Owner` object from another `_Owner` object performs a **move**.
- After initialization, the source is **moved**.
- Copy-initializing an `_Owner` object is **invalid**.

### 5.5 Scope Exit Requirements

- `_Ctor` object — must be **initialized** before leaving its scope.
- `_Dtor` object — must be **uninitialized** before leaving its scope.
- All other objects — no required exit state.

---

## 6. Function Call Arguments

Passing an argument to a function is subject to the same source-validity rules as return (§3)
and initialization (§5). The analyser runs checks in a two-pass scheme: expressions are
evaluated first, then diagnostics are emitted per-parameter so that ordering effects
(e.g. `f(p, p->field)`) are handled correctly.

### 6.1 Source Validity

The argument must not be **uninitialized, expired, or moved**. The single exception is the
`_Ctor` case described in §6.2.

```c
// INVALID — passing uninitialized argument
int x;
use(x);   // diagnostic: object is uninitialized

// INVALID — passing expired argument
struct string * _Owner p = make_string();
string_destroy(p);     // p is now expired
print_string(p);       // diagnostic: object lifetime ended
```

### 6.2 `_Ctor` Exception

When the parameter type is a pointer to a `_Ctor` object, the callee is expected to
**initialize** the pointed object. The analyser therefore:

- Does **not** emit an uninitialized diagnostic for the pointed object.
- Resets the pointed object to **ANY** after the call (the constructor has initialized it).
- Still checks that the *pointer itself* is valid (not null when not `_Opt`, not expired, not moved).

```c
// VALID — passing uninitialized object to constructor
struct string s;         // s is UNINITIALIZED
string_init(&s);         // &s is valid; pointed object uninitialized is OK here
                         // after call: s is ANY (initialized by callee)
print_string(&s);        // OK: s is now ANY
```

### 6.3 Null Safety  *(nullable_enabled)*

- Passing a possibly-null pointer to a non-`_Opt` parameter is **invalid**.
- Passing to an `_Opt` parameter is valid, but the pointer must not be moved or expired.

```c
void use(struct string * p);   // p is non-_Opt

struct string * _Opt q = get_opt();
use(q);   // INVALID: diagnostic: passing possibly null pointer to non-nullable parameter

if (q != NULL)
    use(q);   // VALID: q is known non-null in this branch
```

### 6.4 Ownership Transfer  *(ownership_enabled)*

- Passing an `_Owner` argument transfers ownership to the callee.
- After the call, the argument object is **expired**.
- The argument must not already be moved, expired, or uninitialized before the call.
- Passing an `_Owner` source to a non-`_Owner` parameter is **invalid** (resource leak).
- If the parameter is an `_Owner` pointer, the pointed object also becomes **expired** after the call.

```c
void take_string(struct string * _Owner p);

struct string * _Owner s = make_string();
take_string(s);    // ownership moves to callee; s is now expired

take_string(s);    // INVALID: diagnostic: passing a moved _Owner object
```

### 6.5 Destructor Parameters  *(ownership_enabled)*

When the parameter type is a pointer to a `_Dtor` object, the callee destroys the contents
of the pointed object:

- The pointed object becomes **expired** after the call.
- The pointer variable itself remains live — the caller is responsible for freeing it.

```c
void string_destroy(struct string * _Dtor p);

struct string * _Owner p = malloc(sizeof *p);
string_init(p);
string_destroy(p);   // pointed object expired; pointer p still valid
free(p);             // OK: pointer variable is still live
```

### 6.6 Pointed-Object State After Call

For any pointer argument — owner or not, `_Ctor` or not — the analyser resets every
concrete object the pointer may alias to **ANY** after the call, because the callee may
have modified the pointed object.

---

## 7. Common Patterns

### 7.1 Constructor / Destructor Pair

```c
struct point {
    _Owner int * data;
    int count;
};

void point_init(struct point * _Ctor p, int n)
{
    p->data  = malloc(n * sizeof(int));
    p->count = n;
}

void point_destroy(struct point * _Dtor p)
{
    free(p->data);  // p->data expires
    p->count = 0;
}

void example(void)
{
    struct point pt;
    point_init(&pt, 10);
    /* use pt ... */
    point_destroy(&pt);
    // OK: _Owner member is expired at scope exit
}
```

### 7.2 Transferring Ownership Through a Function

```c
struct string * _Owner clone(const struct string * src)
{
    struct string * _Owner result = malloc(sizeof *result);
    string_init(result);
    /* copy src contents into result ... */
    return result;   // ownership moves to caller
}

void caller(void)
{
    struct string * _Owner a = make_string();
    struct string * _Owner b = clone(a);   // a unchanged; b is new owner
    string_destroy(a); free(a);
    string_destroy(b); free(b);
}
```

### 7.3 Nullable Pointer with Null Check

```c
void process(struct string * _Opt _Owner p)
{
    if (p == NULL)
        return;
    // p is known non-null in this branch
    print_string(p);
    string_destroy(p);
    free(p);
}
```

### 7.4 Reassigning an `_Owner` Pointer

Must release the current resource before reassigning:

```c
struct string * _Owner p = make_string();

string_destroy(p);
free(p);
p = NULL;

p = make_string();   // OK: destination is null
```

Forgetting the release triggers: *"destination `_Owner` object is not null, uninitialized,
or moved before assignment (potential resource leak)"*.

---

## 8. Diagnostic Reference

| Diagnostic | Trigger | Example |
|---|---|---|
| `W_FLOW_UNINITIALIZED` | Source is (or may be) uninitialized | `return x;` where `x` was never set |
| `W_FLOW_LIFETIME_ENDED` | Source is expired or moved; or dest `_Owner` is live on overwrite | `p = q;` where `q` was already moved |
| `W_FLOW_NULL_DEREFERENCE` | Possibly-null pointer passed or assigned to non-`_Opt` target | `use(opt_p);` where `opt_p` may be null |
| `W_FLOW_NON_NULL` | Condition is statically always true or always false | `if (p)` where `p` is always non-null |