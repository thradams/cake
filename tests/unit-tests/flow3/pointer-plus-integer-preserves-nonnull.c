#pragma safety enable

/* `ptr + int`, `int + ptr` and `ptr - int` keep the pointer's alternative (not `ptr - ptr`), so `current->lexeme + 0` stays non-null */

void f1(char* p)
{
    unsigned char* psz = (unsigned char*)(p + 0);
    *psz = 1; /* ok: p + 0 is still non-null */
}

void f2(char* p)
{
    unsigned char* psz = (unsigned char*)(p + 5);
    *psz = 1; /* ok: same rule with a nonzero offset */
}

void f3(char* p)
{
    unsigned char* psz = (unsigned char*)(p - 1);
    *psz = 1; /* ok: same rule for subtraction */
}

void f4(char* p)
{
    unsigned char* psz = (unsigned char*)(3 + p);
    *psz = 1; /* ok: same rule, integer on the left */
}

/* contrast: an _Opt pointer's uncertainty survives pointer + integer */
void opt_still_warns(char* _Opt p)
{
    unsigned char* psz = (unsigned char*)(p + 0); //lint 35 passing a possible null pointer '(unsignedchar*)(p+0)' to non-nullable pointer parameter
    *psz = 1; //lint 33 possible null pointer dereference
}
