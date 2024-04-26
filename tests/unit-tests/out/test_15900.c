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
