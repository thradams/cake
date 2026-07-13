

#pragma flow enable

// help.mdcvxcv

void f1(int * p)
{
    if (p)
    {
        compile_assert(p != 0);
    }
    else
    {
        compile_assert(p == 0);   /* ok */
    }
}

void f2(void)
{
    int a = 1;
    compile_assert(a == 1);   /* ok */
    compile_assert(a != 0);   /* ok */
}

void f3(int c)
{
    int a;
    if (c)
        a = 1;
    else
        a = 2;
    compile_assert(a == 1 || a == 2);   /* ok — both alternatives satisfy */
    compile_assert(a != 0);             /* ok — neither 1 nor 2 is zero   */
}

void f4(void)
{
    int a;
//    compile_assert(a != 0);   /* warning: uninitialized use; compile_state failed */
}


void f5(int x)
{
//    compile_assert(x != 0);   /* compile_state any */
}

void f6(int x)
{
    if (x != 2)
    {
        compile_assert(x != 2);   /* ok — NOT_EQUAL 2, definitely not 2 */
      //  compile_assert(x != 0);   /* compile_state any — 0 is still possible */
    }
}


void f7(int * p)
{
    if (!p)
    {
        compile_assert(p == 0);   /* ok */
    }
}

void f8(int c1, int c2)
{
    int a, b;
    if (c1) a = 1; else a = 0;
    if (c2) b = 1; else b = 0;

    compile_assert(a == 0 || a == 1);   /* ok — exhaustive */
    compile_assert(a || b);             /* compile_state any — {0,0} fails */
}

void f9(int * _Opt p)
{
    if (p != 0)
    {
        compile_assert(p != 0);   /* ok */
    }
    else
    {
        compile_assert(p == 0);   /* ok */
    }
}


void f10(int x)
{
    if (x == 3)
    {
        compile_assert(x == 3);   /* ok — EQUAL 3 */
        compile_assert(x != 0);   /* ok — EQUAL 3, nonzero */
        compile_assert(x != 4);   /* ok — EQUAL 3, not 4 */
    }
}

