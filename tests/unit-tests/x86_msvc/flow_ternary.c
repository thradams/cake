/* Cake 0.12.26 x86_msvc */
struct X {
    char * p;
};


struct X f();
void free(void * p);

int main()
{
    struct X  x;

    x = 1 ? f() : f();
    free(x.p);
}


