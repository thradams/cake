#pragma safety enable



union X {
    struct {
        int a, b;
        union {
            double d;
        } z;
    } y;
    double d;
    char c;
};
static_assert(sizeof(union X) == 16);