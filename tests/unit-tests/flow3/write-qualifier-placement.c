#pragma safety enable

/* _Clear/_Dtor/_Out/_Uninitialized describe the pointee: on a non-pointer is error 1940, with a const pointee error 1930, in both declarators and parameters */

struct S { int i; };

/* documented rejections: error 1940 (`_Out int c;`, `void p4(_Uninitialized int x)`) and error 1930 (`const _Clear struct S* p`, ...) */

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
