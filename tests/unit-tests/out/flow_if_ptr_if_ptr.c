struct Z {
    int i;
};

struct X {
    struct Y * pY;
};

struct Y {
    struct Z * pZ;
};


void f(struct X * p)
{
    if (p && p->pY && p->pY->pZ)
    {
        p->pY->pZ->i = 1;
    }
}

