#pragma safety enable

/* `catch { delete(p); p = 0; } if (p && p->m)` must not report an ended pointee: only alternatives that can coexist with the dereferenced pointer count (parser.c, 250 false 31s) */

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long s);

struct D { int x; };
struct I { struct D* _Owner _Opt p_declarator; };

void init_declarator_delete(struct I* _Owner _Opt p);
void use(struct D* p);
int maybe(void);

struct I* _Owner _Opt f(void)
{
    struct I* _Owner _Opt p = calloc(1, sizeof(struct I));
    try
    {
        if (p == 0) throw;
        if (maybe()) throw;
    }
    catch
    {
        /* releases the object and nulls the pointer, the null makes the guard below safe */
        init_declarator_delete(p);
        p = 0;
    }

    /* must NOT warn: the only arm that ended it is the one `p &&` excludes */
    if (p && p->p_declarator)
    {
        /* argument passing goes through the same origin filter, must NOT warn */
        use(p->p_declarator);
    }

    return p;
}
