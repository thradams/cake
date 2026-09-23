struct X
{
    char s;
    //align member 7
    double c;
    int s2;
    //align member 4
};

int main(void)
{
    static_assert(_Alignof(float[10]) == alignof(float));
    static_assert(alignof(char) == 1);
    static_assert(sizeof(struct X) == 24);
    static_assert(alignof(struct X) == 8);
}
