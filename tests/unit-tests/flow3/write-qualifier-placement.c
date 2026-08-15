#pragma safety enable

/*
   Placement rule for the state/write qualifiers: _Clear, _Dtor, _Out,
   _Uninitialized.

   Each describes what happens to the POINTED-TO object, so:

     - putting one on a non-pointer is meaningless   -> error 1940
     - the pointee cannot also be const (the callee
       has to write it)                              -> error 1930

   This used to be split across two places with different coverage: parser.c
   checked variable declarators (_Dtor/_Uninitialized/_Clear only) and flow3
   checked parameters (_Clear/_Dtor/_Out only). So `_Out int x;` and
   `void f(_Uninitialized int x)` both slipped through. Both positions now share
   one helper in flow3, and all four qualifiers are covered in each.

   Only the legitimate uses below are compiled -- the rejected forms are
   documented rather than written, so this file stays clean.
*/

struct S { int i; };

/*
   Documented rejections (error 1940 -- qualifier on a non-pointer):

       _Clear          int a;   _Dtor int b;   _Out int c;   _Uninitialized int d;
       void p1(_Clear         int x);
       void p2(_Dtor          int x);
       void p3(_Out          int x);
       void p4(_Uninitialized int x);   // was NOT caught before

   Documented rejections (error 1930 -- pointee cannot also be const):

       void c1(const _Clear          struct S* p);
       void c2(const _Dtor           struct S* p);
       void c3(const _Out           struct S* p);
       void c4(const _Uninitialized  struct S* p);   // was NOT caught before
*/

/* --- legitimate uses: the qualifier sits on the pointed object --- */

void clear_it(_Clear struct S* p);      /* callee zeroes *p            */
void ctor_it(_Out struct S* p);        /* callee constructs *p        */
void dtor_it(_Dtor struct S* p);        /* callee destroys *p          */

/* Return position: the allocation comes back already zeroed / indeterminate. */
struct S* _Owner _Opt _Clear          alloc_zeroed(unsigned long n);
struct S* _Owner _Opt _Uninitialized  alloc_raw(unsigned long n);

void free(void* _Owner _Opt p);

void use(void)
{
    struct S* _Owner _Opt z = alloc_zeroed(1);
    if (z)
    {
        clear_it(z);
        free(z);
    }
}
