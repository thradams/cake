struct X
{
    char s;
    double c;
    char s2;
};
static_assert(alignof(struct X) == 8);
static_assert(sizeof(struct X) == 24);