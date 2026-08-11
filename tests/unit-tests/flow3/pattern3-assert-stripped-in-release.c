#pragma safety enable

/*
   PATTERN 3 -- warnings caused by the standard `assert` disappearing in a
   RELEASE build, and how _Assert fixes them.

   `assert` is a MACRO. With a real <assert.h> in a release build (NDEBUG) it
   expands to `((void)0)` -- the check vanishes, flow3 has no invariant to
   narrow on, and the following `p->member` reads possibly-null and warns.

   _Assert is a cake KEYWORD, so no header macro can shadow or strip it:
   it narrows in every build, including release. This is the fix for the
   assert-guarded warning-33s seen when dogfooding expressions.c.

   Function A (_Assert) is clean; B (stripped assert) reproduces the
   warning.
*/

#define NULL ((void*)0)
#define assert(x) ((void)0)          /* release: standard assert does nothing */

struct spec { int flags; };
struct decl { struct spec* _Opt declaration_specifiers; };

/* A: _Assert narrows even though `assert` is stripped -> clean. */
void A(struct decl* _Opt p)
{
    _Assert(p != NULL);
    if (p->declaration_specifiers) { }        /* clean */
}

/* B: the stripped assert macro does nothing -> narrowing lost -> warns. */
void B(struct decl* _Opt p)
{
    assert(p != NULL);                        /* does nothing now */
    if (p->declaration_specifiers) { }        /* warns: -> on possibly-null p */ //lint 33 -> operator applied to a possible null pointer 'p'
}
