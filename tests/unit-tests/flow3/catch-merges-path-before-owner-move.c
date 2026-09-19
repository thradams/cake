#pragma safety enable

/* FIXED: each throw joins the catch under its own snapshot map, so "p named this object" and "that object ended" from different paths are not paired (expressions.c compound literal) */

struct expression { int kind; };

void* _Owner _Opt calloc(int n, unsigned long size);
void expression_delete(struct expression* _Owner _Opt p);
struct expression* _Owner _Opt tail(struct expression* _Owner p);
int step(void);

struct expression* _Owner _Opt compound_literal(void)
{
    struct expression* _Owner _Opt p = 0;
    try
    {
        p = calloc(1, sizeof * p);
        if (p == 0)
            throw;

        if (step() != 0)
            throw;

        p = tail(p);
        if (p == 0)
            throw;
    }
    catch
    {
        expression_delete(p);
        p = 0;
    }
    return p;
}
