/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * text;
};

struct X makex();
void clear(struct X * p);

void f(struct X * pX)
{
    struct X x;

    x = makex();
    clear(&x);
    ;
}
