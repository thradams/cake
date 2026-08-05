#pragma safety enable

/*
   `throw` (like `return`) ends the current path -- code after an
   `if (cond) { ...; throw; }` with no `else` only ever runs when the
   condition was false, so a variable assigned right after such a guard,
   in both arms of a later if/else, must be seen as definitely assigned
   by the time it's used.

   Modeled after a real shape in cake's own unary_expression
   (expressions.c, around line 4086): an early-validation `if` that
   throws on failure, followed by an if/else that unconditionally
   assigns a pointer (via `&member`, which is always non-null) in both
   arms.
*/

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
