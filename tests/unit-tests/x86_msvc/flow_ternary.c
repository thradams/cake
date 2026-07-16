/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * p;
};

struct X f();
void free(void * p);

int main()
{
    struct X x;

    x = 1 ? f() : f();
    free(x.p);
}
