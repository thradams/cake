#pragma safety enable

/*
   _Assert -- an assertion with compile_assert's grammar that flow3
   ALWAYS narrows on, AND that lowers to a real runtime assert().

   The standard `assert` is a macro: with a real <assert.h> in a release build
   (NDEBUG) it expands to `((void)0)`, so flow3 loses the invariant and reports
   false positives (see pattern3-assert-stripped-in-release.c).

   _Assert is a cake keyword, so no header macro can shadow or strip it:
     - flow3 narrows on it identically to `assert` (a bare pointer, an explicit
       comparison, a conjunction, a value range) in EVERY build, incl. release;
     - it has compile_assert's grammar and, like compile_assert, is an
       EXPRESSION -- usable in expression position, not only as a statement:
           _Assert ( condition );
           _Assert ( condition , "message" );
           x = (_Assert(condition), value);      // expression form
     - code generation is controlled by the -runtime-asserts flag:
         * WITHOUT the flag: no runtime code (like compile_assert) -- only the
           compile-time flow3 narrowing.
         * WITH -runtime-asserts: it lowers to an expression that, on failure,
           calls an emitted handler (generated like the memcpy helper, no
           <assert.h>) passing the file name, line number and message text:
               (condition) ? (void)0 : <handler>("file", line, "message")

   Below, the `assert` macro is deliberately stripped to simulate release; every
   _Assert still narrows, so the file is clean.
*/

#define NULL ((void*)0)
#define assert(x) ((void)0)          /* release: standard assert does nothing */

struct X { struct X* _Opt next; int i; };

/* Explicit comparison. */
void compare(struct X* _Opt p)
{
    _Assert(p != NULL);
    p->i = 1;                        /* ok */
}

/* Conjunction narrows every conjunct. */
void conjunction(struct X* _Opt p)
{
    _Assert(p && p->next);
    p->i = 1;                        /* ok */
    p->next->i = 2;                  /* ok */
}

/* Value range, with a message (compile_assert grammar). */
void range(int n)
{
    _Assert(n > 0, "n must be positive");
    compile_assert(n > 0);           /* provable from the _Assert */
}

/* Expression position: _Assert used inside a larger expression (here a
   comma expression). The narrowing applies and persists afterward. */
int in_expression(struct X* _Opt p)
{
    (_Assert(p != NULL), (void)0);
    return p->i;                     /* ok: p was narrowed non-null */
}
