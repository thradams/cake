#pragma safety enable

/* three non-restrict parameters of the same type: every pair may be the same object (5 partitions) */

#define NULL ((void*)0)

struct Y { int i; };
struct X { struct Y* pY; };

/*
    entry flow, the same for every function in this file:

    p1 = new();

    if (p2 == p1) p2 = p1;
    else          p2 = new();

    if (p3 == p1)      p3 = p1;
    else if (p3 == p2) p3 = p2;
    else               p3 = new();

    which are the 5 partitions:

    p1 == p2 == p3
    p1 == p2, p3 different
    p1 == p3, p2 different
    p2 == p3, p1 different
    all different
*/

/* must warn: p1 == p3 */
void p1_p3_same(struct X* p1, struct X* p2, struct X* p3, struct Y* y)
{
    p3->pY = y;
    p1->pY = NULL;
    p3->pY->i = 1; //lint 100 in case p3 == p1
}

/* must warn: p2 == p3 */
void p2_p3_same(struct X* p1, struct X* p2, struct X* p3, struct Y* y)
{
    p3->pY = y;
    p2->pY = NULL;
    p3->pY->i = 1; //lint 100 in case p3 == p2
}

/* must warn: p1 == p2 == p3, the last write wins through any of them */
void all_same(struct X* p1, struct X* p2, struct X* p3, struct Y* y)
{
    p1->pY = y;
    p2->pY = y;
    p3->pY = NULL;
    p1->pY->i = 1; //lint 100 in case p3 == p1
}

/* Must NOT warn: every parameter's member is set after the NULL write. */
void all_set_after(struct X* p1, struct X* p2, struct X* p3, struct Y* y)
{
    p1->pY = NULL;
    p1->pY = y;
    p2->pY = y;
    p3->pY = y;
    p1->pY->i = 1;
    p2->pY->i = 1;
    p3->pY->i = 1;
}
