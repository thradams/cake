#pragma safety enable
/* p + k on a pointer to an array element points to the element k ahead */
void f(int c)
{
    int a[3] = { 0 };
    int *p = &a[0];
    int *r = p + 1;
    compile_assert(r == &a[1]);
    int *s = a + 2;
    compile_assert(s == &a[2]);
    int *t = r - 1;
    compile_assert(t == &a[0]);
    int *u = &a[2] - 2;
    compile_assert(u == &a[0]);
    int *v = p + 3;
    compile_assert(v != 0);
    int *w = p + c;
    compile_assert(w != 0);
}
