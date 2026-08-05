#pragma safety enable

/*
   _Clear and _Uninitialized in RETURN position describe the CONTENTS of the
   returned (allocated) region:

     _Clear         the returned pointee is all-zero      (e.g. calloc)
     _Uninitialized the returned pointee is indeterminate (e.g. malloc)

   _Clear is the same qualifier used on a parameter (where it means "the callee
   zeroes the pointee"); in return position it states the returned region comes
   back already zeroed -- so one keyword covers both readings.

   With _Clear, flow3 seeds each member of the returned pointee to a concrete 0,
   so `p->m == 0` is provably true (rather than relying on the old "empty
   operand is vacuously true" calloc crutch). With _Uninitialized, members are
   modeled as uninitialized, so reading one before writing it warns -- but
   merely holding the pointer (and writing through it) is fine.

   The qualifier sits next to the other return qualifiers on the pointer
   (`T* _Owner _Opt _Clear f()`).
*/

#define NULL ((void*)0)

struct X { int a; int b; };

struct X* _Owner _Opt _Clear         my_calloc(void);
struct X* _Owner _Opt _Uninitialized my_malloc(void);
void free(void* _Owner _Opt p);
void use(int);

/* _Clear: members are concretely zero. */
void zeroed(void)
{
    struct X* _Owner _Opt x = my_calloc();
    if (x == NULL) return;
    use(x->a);            /* ok: a is zero-initialized (no uninit warning) */
    free(x);
}

/* _Uninitialized: writing first is required before reading. */
void malloced_ok(void)
{
    struct X* _Owner _Opt x = my_malloc();
    if (x == NULL) return;
    x->a = 1;             /* initialize before use */
    use(x->a);            /* ok */
    free(x);
}

/*
   Documented (these warn, by design):

     void malloced_bad(void) {
         struct X* _Owner _Opt x = my_malloc();
         if (x == NULL) return;
         use(x->a);        // warns: reading an uninitialized member
         free(x);
     }
*/
