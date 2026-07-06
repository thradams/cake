/* Cake 0.14.05 x86_msvc */

struct X {
    char * owner_variable;
};
struct X global;

char *f()
{
    return global.owner_variable;
}
