#pragma safety enable

/* operands of sizeof / _Alignof and the expression of a static/compile assert never run, so no null-deref or division-by-zero is reported there */

struct X { int a; };

/* sizeof(*p) / sizeof(p->a): p is never dereferenced. */
void sizeof_deref(int* _Opt p, struct X* _Opt q)
{
    (void)sizeof(*p);
    (void)sizeof(p[0]);
    (void)sizeof(q->a);
    unsigned long n = sizeof(*p) + sizeof(q->a);
    (void)n;
    /* Real dereference DOES warn:   *p = 1;   q->a = 1; */
}

/* Division / modulo by zero inside sizeof is likewise not evaluated. */
void sizeof_divzero(int n)
{
    (void)sizeof(1 / 0);
    (void)sizeof(n / 0);
    (void)sizeof(n % 0);
    /* Real division DOES warn:   int x = n / 0; */
}

/* the same suppression inside a static/compile assert: only the assert's own result is reported */
