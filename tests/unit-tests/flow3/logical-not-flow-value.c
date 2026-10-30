#pragma safety enable

/* !x of a value known only by flow */

void known_zero(int c)
{
    int a = 1;
    if (c) { a = 0; } else { a = 0; }
    compile_assert(!a);
}

void known_nonzero(int c)
{
    int a = 0;
    if (c) { a = 2; } else { a = 3; }
    compile_assert(!a == 0);
}

void per_path(int c)
{
    int a = c ? 0 : 1;
    int b = !a;
    if (a)
    {
        compile_assert(b == 0);
    }
    else
    {
        compile_assert(b == 1);
    }
}
