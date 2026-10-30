#pragma safety enable

/* issue #515 */
struct S { int* _Nonnull q; };

int f(void)
{
    struct S s;
    return *s.q; //lint 30 using a possibly uninitialized object 's.q'
}
