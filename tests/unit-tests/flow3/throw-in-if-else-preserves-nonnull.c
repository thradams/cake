#pragma safety enable

/* code after `if (cond) { ...; throw; }` only runs when cond was false, so a pointer assigned in both arms of a later if/else is definitely assigned (expressions.c unary_expression) */

struct S { int x; };
void g(int* p);
int cond1(void);
int cond2(void);

void f(struct S* s)
{
    try
    {
        int* _Opt p = 0;

        if (cond1())
        {
            if (cond2())
            {
                throw; /* ends this path -- nothing below runs when taken */
            }

            p = &s->x; /* only reached once cond2() was proven false */
        }
        else
        {
            p = &s->x;
        }

        g(p); /* ok: both arms of the if/else assign a non-null address */
    }
    catch
    {
    }
}
