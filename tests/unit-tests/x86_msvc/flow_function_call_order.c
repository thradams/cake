/* Cake 0.14.05 x86_msvc */

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
