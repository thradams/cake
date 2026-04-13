
struct X {
    int i;
    double data[];
};

static_assert(sizeof(struct X) == 8);
