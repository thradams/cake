union U {
    unsigned int _[2];
    int i;
    double d;
};

constexpr union U u1 = {1, 2}; /*1.2*/
static_assert(u1._[0] == 1);
static_assert(u1._[1] == 2);
