/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct X {
    char * text;
};

struct Y {
    struct X x;
};

struct Y f();

int main()
{
    struct Y y;

    y = f();
}
