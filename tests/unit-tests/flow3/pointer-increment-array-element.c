#pragma safety enable
/* ++, --, += and -= on a pointer to an array element move it to the real element */
void f(int c)
{
    int a[3] = { 0 };
    int *p = &a[0];
    p++;
    compile_assert(p == &a[1]);
    ++p;
    compile_assert(p == &a[2]);
    p--;
    compile_assert(p == &a[1]);
    p += 1;
    compile_assert(p == &a[2]);
    p -= 2;
    compile_assert(p == &a[0]);
    int *q = p++;
    compile_assert(q == &a[0]);
    compile_assert(p == &a[1]);
    p += 2;
    compile_assert(p != 0); /* one past the end: unknown target */
    p = &a[0];
    p += c;
    compile_assert(p != 0); /* c unknown */
}
