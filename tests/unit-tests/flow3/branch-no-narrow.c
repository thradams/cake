#pragma safety enable

/* known issue: a non-narrowing condition uses the identity pair, so `a = 1` in the then-block leaks after the if */

int unknown();

void no_narrow_if()
{
    int a = 0;
    if (unknown())
    {
        a = 1;
    }
    // static_debug(a);
    /* Both alternatives must survive the join. */
    compile_assert(a == 0 || a == 1);
//    compile_assert(a != 2);


   // compile_assert(a == 1);
    
}

void no_narrow_if_with_else()
{
    int a = 0;
    if (unknown())
    {
        a = 1;
    }
    else
    {
        a = 2;
    }
    compile_assert(a == 1 || a == 2);

}
