/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct Y {
    int a;
    int ar[3];
    int b;
};

void T2()
{
    struct Y y;

    y.a = 0;
    y.ar[0] = 0;
    y.ar[1] = 2;
    y.ar[2] = 3;
    y.b = 4;
}
