/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

struct X f();

struct X f2()
{
    return f();
}
