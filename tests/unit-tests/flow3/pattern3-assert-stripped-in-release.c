#pragma safety enable

/* `assert` vanishes in a release build and the following p->member warns; _Assert is a keyword and narrows in every build */

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
