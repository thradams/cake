#pragma safety enable

/* `a += 1` keeps a pointer's alternative like ++ does, so `*a = 1` after it must not warn */

void f(int* a)
{
    a += 1;
    *a = 1; /* ok: += on a non-optional pointer keeps it non-null */
}

void g(int* a)
{
    a -= 1;
    *a = 1; /* ok: same rule for -= */
}

void h(int* a, int n)
{
    a += n;
    *a = 1; /* ok: same rule with a non-constant offset */
}

/* contrast: an _Opt pointer's uncertainty survives the compound assignment */
void opt_still_warns(int* _Opt a)
{
    a += 1;
    *a = 1; //lint 33 possible null pointer dereference
}
