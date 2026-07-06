/* Cake x64_msvc */

union __tag0 {
    int x;
};

struct X {
    union __tag0 __m0;
};

int main()
{
    struct X x;
    struct X __a;

    __a..x = 50;
    x = __a;
}
