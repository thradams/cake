#pragma safety enable

/* _Opt parameters: only the non-null ones can be the same object (15 cases for three) */

#define NULL ((void*)0)

struct Y { int i; };
struct X { struct Y* pY; };

/*
    entry flow:

    if (not null) p1 = new(); else p1 = 0;

    if (p2 == p1) p2 = p1;     (both null, or both the same object)
    else if (not null) p2 = new(); else p2 = 0;
*/
void two_opt_same(struct X* _Opt p1, struct X* _Opt p2, struct Y* y)
{
    if (p1 && p2)
    {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1; //lint 100 in case p2 == p1
    }
}

/*
    entry flow, the same for the next two functions:

    if (not null) p1 = new(); else p1 = 0;

    if (p2 == p1) p2 = p1;
    else if (not null) p2 = new(); else p2 = 0;

    if (p3 == p1) p3 = p1;
    else if (p3 == p2) p3 = p2;
    else if (not null) p3 = new(); else p3 = 0;
*/

/* must warn: three _Opt, p1 == p3 */
void three_opt_p1_p3_same(struct X* _Opt p1, struct X* _Opt p2, struct X* _Opt p3, struct Y* y)
{
    if (p1 && p2 && p3)
    {
        p3->pY = y;
        p2->pY = y;
        p1->pY = NULL;
        p3->pY->i = 1; //lint 100 in case p3 == p1
    }
}

/* must warn: p2 is not used, p1 == p3 still possible */
void three_opt_p2_unused(struct X* _Opt p1, struct X* _Opt p2, struct X* _Opt p3, struct Y* y)
{
    if (p1 && p3)
    {
        p3->pY = y;
        p1->pY = NULL;
        p3->pY->i = 1; //lint 100 in case p3 == p1
    }
    (void)p2;
}

/*
    entry flow:

    if (not null) p1 = new(); else p1 = 0;

    if (p2 == p1) p2 = p1;     (only when p1 is not null: p2 is never null)
    else          p2 = new();

    Must NOT warn: inside if (p1 == NULL), p2 cannot be p1.
*/
void opt_null_does_not_alias(struct X* _Opt p1, struct X* p2, struct Y* y)
{
    p2->pY = y;
    if (p1 == NULL)
    {
        p2->pY->i = 1;
    }
}

/*
    entry flow:

    if (not null) p1 = new(); else p1 = 0;     (restrict: no if (p2 == p1))
    if (not null) p2 = new(); else p2 = 0;
*/
void opt_restrict(struct X* _Opt restrict p1, struct X* _Opt p2, struct Y* y)
{
    if (p1 && p2)
    {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1;
    }
}
