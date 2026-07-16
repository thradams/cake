/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct Y {
    char * owner_variable;
    char * non_owner_variable;
};

struct X {
    char * owner_variable;
    char * non_owner_variable;
    struct Y y1;
    struct Y y2;
};

void f()
{
    struct X x;
    struct X x2;
    char * p;

}
