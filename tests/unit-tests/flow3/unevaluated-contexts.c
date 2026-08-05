#pragma safety enable

/*
   Unevaluated operands never run, so flow3 must not report runtime hazards
   inside them. The operand of sizeof / _Alignof, and the expression inside a
   static/compile assert, are analyzed for their TYPE/VALUE only -- the code is
   never executed -- so a possibly-null dereference or a division by zero there
   is not a real fault.

   This file is clean: none of the hazards below are reported. The same
   constructs OUTSIDE an unevaluated context (shown in comments) do warn.
*/

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

/*
   The same suppression applies to the expression inside a static/compile
   assert (also an unevaluated, compile-time flow check): a null-deref or
   div-by-zero there is not reported as a runtime hazard -- only the assert's
   own pass/fail result is.
*/
