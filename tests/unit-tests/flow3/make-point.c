#pragma safety enable

/*
   A make_point(int x, int y) "constructor": build a struct from parameters.

   This sample records what flow3 tracks through struct construction today.
   Constant members are seeded from the object and are provable; construction
   from non-constant parameters and values across a call are NOT yet tracked
   (see the limitations at the bottom).

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

struct point { int x, y; };

/* Build from parameters. Nothing concrete is provable about the members here
   (the members take the parameters' unknown values), so no assert is made --
   this just exercises the construction path cleanly. */
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

/* Aggregate init from a NON-CONSTANT integer expression now carries the
   relation, just like scalar init. */
void member_from_narrowed_param(int x)
{
    if (x < 0)
        x = 0;
    struct point p = { x, 0 };
    compile_assert(p.x >= 0);          /* member relation is carried */
}

/*
   KNOWN LIMITATION (documented, not asserted):

   Values are not propagated across a call (no interprocedural analysis):

          struct point q = make_point(3, 4);
          compile_assert(q.x == 3);      // FAILS: call result is unknown
*/
