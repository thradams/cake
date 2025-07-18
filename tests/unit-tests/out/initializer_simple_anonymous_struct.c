union _struct_tag_0 {
    int x;
};

struct X {
    union _struct_tag_0  __m0;
};


int main()
{
    struct X  x;
    struct X  __cmp_lt_0;

    __cmp_lt_0..x = 50;
    x = __cmp_lt_0;
}

