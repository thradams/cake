#pragma safety enable

/*
   sizeof / _Alignof / _Countof produce a known constant, seeded into flow3
   from the value the parser already folded. So they can be used in
   flow-checked comparisons (compile_assert), not just C11 static_assert.

   The result type is size_t, so the value is unsigned.

   NOTE: for a variable-length array (VLA) `sizeof` is computed at runtime and
   has no constant value; flow3 correctly leaves it unknown (see vla() below).
*/

void scalar_sizes(void)
{
    compile_assert(sizeof(char) == 1);
    compile_assert(sizeof(int) == 4);
    compile_assert(sizeof(double) == 8);
}

void array_size(void)
{
    int v[10];
    compile_assert(sizeof(v) == 40);       /* 10 * sizeof(int) */
}

void alignment(void)
{
    compile_assert(_Alignof(int) == 4);
}

/* A VLA's sizeof is dynamic: no constant is claimed, so this is simply not
   provable and is left un-asserted (documented, not a bug):

       void vla(int n) {
           int a[n];
           unsigned long s = sizeof(a);      // runtime value, stays unknown
       }
*/
void vla(int n)
{
    int a[n];
    unsigned long s = sizeof(a);
    (void)s;
}
