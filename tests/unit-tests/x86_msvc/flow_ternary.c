/* Cake 0.14.05 x86_msvc */

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
