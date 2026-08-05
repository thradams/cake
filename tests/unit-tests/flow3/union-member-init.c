#pragma safety enable

/*
   A union's members share storage: once any member is initialized, the whole
   union is initialized, so the other members must not be reported as
   "possibly uninitialized".

   flow3 tracked union members like independent struct members, so after
   `struct S s = { 0 };` (which sets the first union member) it reported every
   OTHER union member as uninitialized -- e.g. returning/passing the object
   warned on `s.value.u`, `s.value.d`, etc. Now, if any union member is
   initialized, the siblings are treated as initialized too.
*/

struct object
{
    int state;
    union { long long i; unsigned long long u; long double d; } value;
};

void use(struct object o);

/* Zero-init sets the union storage: no member is uninitialized. */
struct object make_zero(void)
{
    struct object empty = { 0 };
    return empty;                    /* clean */
}

/* Writing one union member initializes the union. */
void write_one(void)
{
    struct object o = { 0 };
    o.value.i = 42;
    use(o);                          /* clean: value.u / value.d not flagged */
}

/*
   A union that is NEVER initialized is still genuinely uninitialized and warns
   (documented, not asserted):

       void fully_uninit(void) {
           struct object o;          // nothing set
           use(o);                   // warns: o.state, o.value.* uninitialized
       }
*/
