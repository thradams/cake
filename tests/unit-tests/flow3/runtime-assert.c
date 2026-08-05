#pragma safety enable

/*
   runtime_assert -- an assertion with compile_assert's grammar that flow3
   ALWAYS narrows on, AND that lowers to a real runtime assert().

   The standard `assert` is a macro: with a real <assert.h> in a release build
   (NDEBUG) it expands to `((void)0)`, so flow3 loses the invariant and reports
   false positives (see pattern3-assert-stripped-in-release.c).

   runtime_assert is a cake keyword, so no header macro can shadow or strip it:
     - flow3 narrows on it identically to `assert` (a bare pointer, an explicit
       comparison, a conjunction, a value range) in EVERY build, incl. release;
     - it has compile_assert's grammar and, like compile_assert, is an
       EXPRESSION -- usable in expression position, not only as a statement:
           runtime_assert ( condition );
           runtime_assert ( condition , "message" );
           x = (runtime_assert(condition), value);      // expression form
     - code generation is controlled by the -runtime-asserts flag:
         * WITHOUT the flag: no runtime code (like compile_assert) -- only the
           compile-time flow3 narrowing.
         * WITH -runtime-asserts: it lowers to an expression that, on failure,
           calls an emitted handler (generated like the memcpy helper, no
           <assert.h>) passing the file name, line number and message text:
               (condition) ? (void)0 : <handler>("file", line, "message")

   Below, the `assert` macro is deliberately stripped to simulate release; every
   runtime_assert still narrows, so the file is clean.
*/

#define NULL ((void*)0)
#define assert(x) ((void)0)          /* release: standard assert does nothing */

struct X { struct X* _Opt next; int i; };

/* Explicit comparison. */
void compare(struct X* _Opt p)
{
    runtime_assert(p != NULL);
    p->i = 1;                        /* ok */
}

/* Conjunction narrows every conjunct. */
void conjunction(struct X* _Opt p)
{
    runtime_assert(p && p->next);
    p->i = 1;                        /* ok */
    p->next->i = 2;                  /* ok */
}

/* Value range, with a message (compile_assert grammar). */
void range(int n)
{
    runtime_assert(n > 0, "n must be positive");
    compile_assert(n > 0);           /* provable from the runtime_assert */
}

/* Expression position: runtime_assert used inside a larger expression (here a
   comma expression). The narrowing applies and persists afterward. */
int in_expression(struct X* _Opt p)
{
    (runtime_assert(p != NULL), (void)0);
    return p->i;                     /* ok: p was narrowed non-null */
}
