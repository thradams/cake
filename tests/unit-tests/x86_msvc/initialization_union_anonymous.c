/* Cake 0.12.26 x86_msvc */
union __tag1 {
    int y;
};

union __tag0 {
    int x;
    int a;
};

struct X {
    union __tag0  __m0;
    union __tag1  __m1;
};


int main()
{
    struct X  x;

    x..x = 0;
    x..a = 0;
    x..y = 50;
}


