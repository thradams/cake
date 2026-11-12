#pragma safety enable
/* forming a pointer outside a[0] .. a + N is undefined behavior */
void f(int c)
{
    int a[3] = { 0 };
    int *p = a;
    *(p + 2) = 1;     /* ok */
    int *e = a + 3;   /* ok: one past the end */
    (void)e;
    *(p + 5) = 1;     //lint 70
    int *q = a + 4;   //lint 70
    (void)q;
    int *r = p - 1;   //lint 70
    (void)r;
    int *s = &a[0];
    s--;              //lint 70
    int *t = &a[2];
    t++;              /* ok: one past */
    t++;              /* t unknown now: nothing */
    int i = c ? 1 : 7;
    int *u = p + i;   //lint 70 the i == 7 path
    (void)u;
    for (int *it = a; it < a + 3; it++) *it = 1; /* ok */
}
