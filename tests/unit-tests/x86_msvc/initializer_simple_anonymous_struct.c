/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

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
