#pragma safety enable

/* p++ / p-- / ++p / --p keep a pointer's alternative: advancing a valid pointer never makes it null, so `a++; *a = 1;` must not warn */

void f(int* a)
{
    a++;
    *a = 1; /* ok: incrementing a non-optional pointer keeps it non-null */
}

void g(int* a)
{
    ++a;
    *a = 1; /* ok: prefix increment, same rule */
}

void h(int* a)
{
    a--;
    *a = 1; /* ok: decrement, same rule */
}

void loop(char* p)
{
    while (*p)
    {
        p++; /* ok: repeated increments still keep p non-null */
    }
}

/* contrast: an _Opt pointer's null-or-not uncertainty survives the increment */
void opt_still_warns(int* _Opt a)
{
    a++;
    *a = 1; //lint 33 possible null pointer dereference
}

/* several increments in a row: the non-null fact propagates across every step */
void many(int* p)
{
    p++;
    p++;
    p++;
    *p = 1; /* ok: still non-null after three increments */
}

void many_mixed(int* p)
{
    p++;
    p++;
    p--;
    *p = 1; /* ok: increments and decrements freely mixed */
}
