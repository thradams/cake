#pragma safety enable

/* two pointer parameters of the same type may point to the same object: a write through one must be seen through the other (p1 == p2) */

#define NULL ((void*)0)

struct Y { int i; };
struct X { struct Y* _Opt pY; };

/*
    entry flow:

    p1 = new();
    if (p2 == p1) p2 = p1;
    else          p2 = new();
*/
void two_params(struct X* p1, struct X* p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1; //lint 100 in case p2 == p1
}

/*
    entry flow:

    if (not null) p1 = new(); else p1 = 0;

    if (p2 == p1) p2 = p1;
    else if (not null) p2 = new(); else p2 = 0;

    if (p3 == p1) p3 = p1;
    else if (p3 == p2) p3 = p2;
    else if (not null) p3 = new(); else p3 = 0;
*/
void three_opt_params(struct X* _Opt p1, struct X* _Opt p2, struct X* _Opt p3, struct Y* y)
{
    if (p1 && p2 && p3)
    {
        p3->pY = y;
        p2->pY = y;
        p1->pY = NULL;
        p3->pY->i = 1; //lint 100 in case p3 == p1
    }
}

/*
    entry flow (restrict: no if (same)):

    p1 = new();
    p2 = new();
*/
void two_params_restrict(struct X* restrict p1, struct X* restrict p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
}

/*
    entry flow (struct X and struct Z cannot be the same object):

    p1 = new();
    p2 = new();
*/
struct Z { struct Y* _Opt pY; };
void different_types(struct X* p1, struct Z* p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
}

/*
    entry flow:

    p1 = new();
    if (p2 == p1) p2 = p1;
    else          p2 = new();

    Must NOT warn: the write through p2 comes last, in both cases p2->pY is y.
*/
void write_order_ok(struct X* p1, struct X* p2, struct Y* y)
{
    p1->pY = NULL;
    p2->pY = y;
    p2->pY->i = 1;
}
