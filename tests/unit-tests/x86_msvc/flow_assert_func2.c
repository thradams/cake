/* Cake 0.12.26 x86_msvc */
struct X {
    char * text;
};


struct X makex();
void clear(struct X * p);

void f(struct X * pX)
{
    struct X  x;

    x = makex();
    clear(&x);
    ;
}


