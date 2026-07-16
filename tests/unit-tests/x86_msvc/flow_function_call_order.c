/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    int * p;
};

struct X *make();
void f(struct X * p, int * p);

int main()
{
    struct X * pX;

    pX = make();
    if (pX->p)
    {
        f(pX, pX->p);
    }
}
