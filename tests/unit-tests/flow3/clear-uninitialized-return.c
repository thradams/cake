#pragma safety enable

/* in return position _Clear means the returned pointee is all zero (calloc) and _Uninitialized means indeterminate (malloc) */

#define NULL ((void*)0)

struct X { int a; int b; };

_Clear struct X* _Owner my_calloc(void);
_Uninitialized struct X* _Owner my_malloc(void);
void free(void* _Owner _Nullable p);
void use(int);

/* _Clear: members are concretely zero. */
void zeroed(void)
{
    struct X* _Owner x = my_calloc();
    if (x == NULL) return;
    use(x->a);            /* ok: a is zero-initialized (no uninit warning) */
    free(x);
}

/* _Uninitialized: writing first is required before reading. */
void malloced_ok(void)
{
    struct X* _Owner x = my_malloc();
    if (x == NULL) return;
    x->a = 1;             /* initialize before use */
    use(x->a);            /* ok */
    free(x);
}

/* documented: reading a member of a _Uninitialized return before writing it warns */
