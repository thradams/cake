#pragma safety enable

struct u { int x; };
struct st { struct u* _Opt u; };
struct sb { struct st* st; };
void use(struct u* p);

void f(struct sb* p)
{
    if (p->st)
    {
        if (p->st->u)
        {
            use(p->st->u); /* no warning: p->st->u was checked */
        }
    }
}

void g(struct sb* p)
{
    if (p->st)
    {
        use(p->st->u); //lint 35
    }
}
