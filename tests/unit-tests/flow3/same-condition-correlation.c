#pragma safety enable

/* two separate ifs on the same predicate `c > 2` share a branch id, so p set under the first is non-null inside the second; the id is dropped once an operand is written */

int f(int c)
{
    int i = 0;
    int* _Opt p = 0;

    if (c > 2) p = &i;
    if (c > 2) i = *p;   /* clean: same predicate proves p is non-null here */

    return i;
}

/* Works through `->` as well. */
struct point { int x, y; };

int g(int c, struct point pt)
{
    struct point* _Opt p = 0;

    if (c > 2) p = &pt;
    if (c > 2) return p->x;   /* clean */

    return 0;
}

/* still warn, correctly: an operand written between the ifs (`c = other()`), or a different predicate (`c > 3`) */
