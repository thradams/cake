#pragma safety enable

/* returns a non-null pointer */
int* f();

int main()
{
    int* p;

    if ((p = f()) == 0) // pointer is always non-null
    {
        /* correctly proven dead: `(p = f()) == 0` resolves to p's real state, f() is non-null so the branch is unreachable */
        compile_assert(p != 0); // unreachable code
    } //lint 68 unreachable code
    else
    {
        compile_assert(p != 0);
    }
}
