/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct Y;

struct X {
    char * text;
    int * p1;
    int i;
    struct Y * pY;
};

struct Y {
    char * p0;
    int * p2;
    double i2;
};

struct X f();
void destroy(struct X * x);

int main()
{
    struct X x;

    x = f();
    destroy(&x);
}
