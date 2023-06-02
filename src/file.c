
struct [[cake::destroy]] X {
    int i;
};

void x_swap(struct X* a, struct X* b)
{
    struct X temp = *a;
    *a = *b;
    *b = [[cake::move]] temp;
}