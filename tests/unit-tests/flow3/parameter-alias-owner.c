#pragma safety enable

/* an _Owner member released through one parameter is released through its alias too */

void free(void* _Owner _Opt ptr);

struct Y { int i; };
struct X { struct Y* _Owner _Opt pY; };

/*
    entry flow:

    p1 = new();
    if (p2 == p1) p2 = p1;
    else          p2 = new();
*/
void freed_through_alias(struct X* p1, struct X* p2)
{
    if (p2->pY)
    {
        free(p1->pY);
        p1->pY = 0;
        p2->pY->i = 1; //lint 100 in case p2 == p1
    }
}

/*
    entry flow (struct X and struct Z cannot be the same object):

    p1 = new();
    p2 = new();
*/
struct Z { struct Y* _Owner _Opt pY; };
void different_types_owner(struct X* p1, struct Z* p2)
{
    if (p2->pY)
    {
        free(p1->pY);
        p1->pY = 0;
        p2->pY->i = 1;
    }
}
