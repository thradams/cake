/* _result is the returned value: each return must satisfy the condition, the caller does not check it */
int f(int a) compile_assert(_result > 0)
{
    if (a)
        return 1;
    return 0; //lint 67
}

int g(int a) compile_assert(_result > 0);

void h(void)
{
    int r = g(1);
    compile_assert(r > 0);
}
