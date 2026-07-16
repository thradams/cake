/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

union __tag1 {
    int y;
};

union __tag0 {
    int x;
    int a;
};

struct X {
    union __tag0 __m0;
    union __tag1 __m1;
};

int main()
{
    struct X x;

    x..x = 0;
    x..a = 0;
    x..y = 50;
}
