#pragma safety enable

/* the non-null contract of a parameter holds at entry; inside the function it is a local */

void g(int* p);

void f(int* p)
{
    p = 0; /* ok: changes only the local copy */
}

void use_after(int* p)
{
    p = 0;
    int i = *p; //lint 33
}

void pass_after(int* p)
{
    p = 0;
    g(p); //lint 35
}

void caller(void)
{
    f(0); //lint 35
}
