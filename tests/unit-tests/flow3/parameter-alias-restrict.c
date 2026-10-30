#pragma safety enable

/* a restrict parameter is not the same object as any other parameter; the others may still alias each other */

#define NULL ((void*)0)

struct Y { int i; };
struct X { struct Y* pY; };

/*
    entry flow:

    p1 = new();
    p2 = new();      (p1 is restrict: no if (p2 == p1))
*/
void restrict_first(struct X* restrict p1, struct X* p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
}

/*
    entry flow:

    p1 = new();
    p2 = new();      (p2 is restrict: one restrict is enough)
*/
void restrict_second(struct X* p1, struct X* restrict p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
}

/*
    entry flow, the same for the next two functions:

    p1 = new();      (restrict: different from p2 and p3)
    p2 = new();

    if (p3 == p2) p3 = p2;
    else          p3 = new();
*/

/* Must NOT warn: p1 is restrict, so p1 != p3, and p2 is not written. */
void restrict_p1_not_p3(struct X* restrict p1, struct X* p2, struct X* p3, struct Y* y)
{
    p3->pY = y;
    p1->pY = NULL;
    p3->pY->i = 1;
}

/* must warn: p1 is restrict, but p2 and p3 may still be the same object */
void restrict_p1_p2_p3_same(struct X* restrict p1, struct X* p2, struct X* p3, struct Y* y)
{
    p3->pY = y;
    p2->pY = NULL;
    p3->pY->i = 1; //lint 100 in case p3 == p2
}

/*
    entry flow:

    p1 = new();
    p2 = new();
    p3 = new();
*/
void all_restrict(struct X* restrict p1, struct X* restrict p2, struct X* restrict p3, struct Y* y)
{
    p3->pY = y;
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
    p3->pY->i = 1;
}
