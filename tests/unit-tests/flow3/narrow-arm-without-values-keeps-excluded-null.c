/* After a `break` only the `continue` path ends the first pass: m->e is null there. On the next pass the false arm of `m->e == 0` has no value of that target, and must not read the null from the parent map */
#pragma safety enable

struct E { int first; };
struct M { const struct E* e; const struct M* parent; };
void g(int);

void f(const struct M* _Nullable map, const struct M* q)
{
    if (map == 0) return;
    for (const struct M* m = map; m && m != q; m = m->parent)
    {
        if (m->e == 0)
            continue;
        g(m->e->first);
        break;
    }
}
