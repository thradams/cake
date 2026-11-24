/* runtime_assert: the caller does not prove it, the body assumes it */
int f(int a) runtime_assert(a > 0);

int g(int a) runtime_assert(a > 0)
{
    compile_assert(a > 0);
    return a;
}

void h(int x)
{
    f(x);
}
