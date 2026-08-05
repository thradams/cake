#pragma safety enable

/*
   Pointer arithmetic can never turn a valid pointer into a null one, so
   `pointer + constant` must stay non-null.

   This holds for a plain pointer parameter (`int* p; p + 1`) AND for an
   array-decay base (`int arr[10]; arr + 3`) -- the latter shows up as a REF to
   the array object, and the additive evaluator now carries the non-null
   through it, producing a non-null pointer result.
*/

/* Plain pointer parameter + constant: correctly non-null. */
void plain_pointer(int* p)
{
    int* q = p + 1;
    compile_assert(q != 0);
    *q = 0;                       /* no warning */
}

/* Minus and inline forms also fine for a plain pointer. */
void plain_pointer_more(int* p)
{
    int* q = p - 1;
    compile_assert(q != 0);
    compile_assert(p + 5 != 0);
}

/* Narrowed optional pointer + constant: also correct. */
void narrowed_opt(int* _Opt p)
{
    if (p != 0)
    {
        int* q = p + 1;
        compile_assert(q != 0);
        *q = 0;
    }
}

/* Array-decay parameter + constant is non-null too. */
void array_decay(int arr[10])
{
    int* q = arr + 3;
    compile_assert(q != 0);
    *q = 0;
    compile_assert(3 + arr != 0);      /* n + arr form */
    compile_assert((arr + 5) - 2 != 0);
}

/* A local array decays the same way. */
void local_array(void)
{
    int a[10];
    int* q = a + 2;
    compile_assert(q != 0);
    *q = 0;
}
