#pragma safety enable

/* `catch { delete(p); p = 0; } if (p && p->m)` must not report an ended pointee: only alternatives that can coexist with the dereferenced pointer count (parser.c, 250 false 31s) */

_Clear void* _Owner calloc(unsigned long n, unsigned long s);

struct D { int x; };
struct I { struct D* _Owner p_declarator; };

void init_declarator_delete(struct I* _Owner _Nullable p);
void use(struct D* p);
int maybe(void);

struct I* _Owner f(void)
{
    struct I* _Owner p = calloc(1, sizeof(struct I));
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
