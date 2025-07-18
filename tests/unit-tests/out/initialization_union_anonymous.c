union _struct_tag_1 {
    int y;
};

union _struct_tag_0 {
    int x;
    int a;
};

struct X {
    union _struct_tag_0  __m0;
    union _struct_tag_1  __m1;
};


int main()
{
    struct X  x;

    x..x = 0;
    x..a = 0;
    x..y = 50;
}

