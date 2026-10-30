#pragma safety enable

/* comparing two parameters selects the alias case: p1 == p2 keeps only the same-object case, p1 != p2 drops it */

#define NULL ((void*)0)

struct Y { int i; };
struct X { struct Y* pY; };

/*
    entry flow, the same for every function in this file:

    p1 = new();
    if (p2 == p1) p2 = p1;
    else          p2 = new();
*/

/* Must NOT warn: p1 != p2, so the write through p1 does not reach p2. */
void not_equal_drops_alias(struct X* p1, struct X* p2, struct Y* y)
{
    if (p1 != p2)
    {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1;
    }
}

/* must warn: p1 == p2, the write through p1 is the write through p2 */
void equal_keeps_alias(struct X* p1, struct X* p2, struct Y* y)
{
    if (p1 == p2)
    {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1; //lint 100 in case p2 == p1
    }
}

/* Must NOT warn: early return on the same-object case. */
void early_return_on_alias(struct X* p1, struct X* p2, struct Y* y)
{
    if (p1 == p2)
        return;
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
}
