# Changelog

Newest version first. Changes not yet in a release go under **Unreleased**.
On release, rename it to the version in `src/version.h` and use the section
as the GitHub Release notes.

## 0.15.20 - 2026-10-11

### Code generation

- `va_start` with one argument (C23) is converted to the old form, using the
  last named parameter. `void f(...)`, with no named parameter, cannot be
  converted and still needs a C23 compiler; warning 126 reports it.
  ```c
  va_start(args);                  // source
  __builtin_va_start(args, count); // generated
  ```

### Fixes

- A whole-number `long double` constant was generated without the decimal
  point (`9223372036854775808L` instead of `9223372036854775808.0L`) on
  targets where `long double` is wider than `double` (gcc/clang x86),
  turning it into an integer constant.
- cake89 failed to link with gcc (`undefined reference to 'truncl'`); cake no
  longer uses `truncl`.

## 0.15.19 - 2026-10-11

### Diagnostics

- A diagnostic inside a `defer` body now has a note showing where the defer
  runs (the `return`, `break`, `continue`, `goto` or end of block). The same
  body runs at every exit and the problem may exist at only one of them.
  ```
  warning 30: using a possibly uninitialized object 'x'
    note: uninitialized from here
    note: defer executed here
     16 |        return 1;
  ```

### Fixes

- Flow analysis ran the defers again at the end of a block that is never
  reached (e.g. after `return`), reporting false `unreachable code` (68) and
  `'p' was moved` (32) inside the defer.

## 0.15.18 - 2026-10-11

### Language

- A `const` object initialized with constants is now a constant, like
  `constexpr`. The same rules apply, but they are reported as warnings
  where `constexpr` reports errors.
  ```c
  const int a[] = { 1, 2, 3 };
  static_assert(a[2] == 3);

  const char s[] = "abc";
  static_assert(s[0] == 'a');
  ```
  - Elements of `const` arrays (including multidimensional arrays and
    strings) are now constants. Before, only scalars and structs were.
  - `const`/`constexpr` objects initialized with a compound literal are
    evaluated as if initialized with its braced initializer:
    `const struct P p = (struct P){ 1, 2 };` makes `p.y` a constant.
- `volatile` objects are never constants, even when `const`. This also
  applies to structs with a `volatile` member.
  `const volatile int x = 3; int a[x];` is now a VLA.

### Diagnostics

- New error **2380**: a `constexpr` pointer must be initialized with a null
  pointer constant (`constexpr int *p = &g;`). `const` pointers are not
  affected.
- Error **1950** (`constexpr`) / warning **74** (`const`), "not exactly
  representable", now also covers:
  - floating to integer: `constexpr int i = 1.5;`
  - integer to floating: `constexpr float f = 16777217;`
- Error **2340** now also rejects `constexpr` objects with `volatile`
  members, at any level, and arrays of them.
- Because `const` tables are now constants, conditions on their elements
  can report warning **44** (conditional expression is constant), as already
  happened with `const` scalars.

### Fixes

- Flow analysis crashed (stack overflow) on `break` or `continue` inside a
  `defer` block (`tests/unit-tests/break_out_of_defer_block.c`).
