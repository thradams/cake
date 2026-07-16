/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * owner_variable;
};
struct X global;

char *f()
{
    return global.owner_variable;
}
