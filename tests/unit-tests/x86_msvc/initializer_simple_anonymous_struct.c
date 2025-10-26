/* Cake 0.12.26 x86_msvc */
union __tag0 {
    int x;
};

struct X {
    union __tag0  __m0;
};


int main()
{
    struct X  x;
    struct X  __v0;

    __v0..x = 50;
    x = __v0;
}


