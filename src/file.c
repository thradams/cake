
#define abs_val(x) _Generic((x),  \
    int:    ((x) < 0 ? -(x) : (x)), \
    double: ((x) < 0 ? -(x) : (x)))

_Static_assert(abs_val(-5)   == 5,   "_Generic abs int");

