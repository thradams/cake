#pragma safety enable

struct Z {
    int i;
};

struct Y {
    int i;
    struct Z* _Opt pZ;
};

struct X {
    struct Y* _Opt pY;
};

void f(struct X* _Opt p)
{
    /*
               &&
             /   \
           &&     \
         /   \     \ 
       p    p->pY   \
                     \
                      p->pY->pZ
    */

    if (p && p->pY && p->pY->pZ)
    {
        p->pY->pZ->i = 1;
    }
}
