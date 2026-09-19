#pragma safety enable

/* struct construction: constant members are provable, construction from parameters and values across a call are not tracked */

struct point { int x, y; };

/* built from parameters: nothing concrete is provable, just exercises the path */
struct point make_point(int x, int y)
{
    struct point p = { x, y };
    return p;
}

/* A constant factory: members are seeded from the object, so they ARE known. */
struct point make_origin(void)
{
    struct point p = { 0, 0 };
    compile_assert(p.x == 0);
    compile_assert(p.y == 0);
    return p;
}

/* Constant construction at a variable declaration is likewise known. */
void uses_constants(void)
{
    struct point p = { 3, 4 };
    compile_assert(p.x == 3);
    compile_assert(p.y == 4);
}

/* aggregate init from a non-constant expression carries the relation like scalar init */
void member_from_narrowed_param(int x)
{
    if (x < 0)
        x = 0;
    struct point p = { x, 0 };
    compile_assert(p.x >= 0);          /* member relation is carried */
}

/* known limitation: `struct point q = make_point(3, 4); compile_assert(q.x == 3)` fails, no interprocedural analysis */
