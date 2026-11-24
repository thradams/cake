/* the body starts as if it had passed through if (condition) */
int f(int a) compile_assert(a > 0)
{
    compile_assert(a > 0);
    return a;
}

int g(int a)
{
    compile_assert(a > 0); //lint 67
    return a;
}
