/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};

struct Y {
    struct X  x;
};


void free(void * ptr);

void f(struct Y * y, struct X * p)
{
    free(y->x.name);
    y->x = *p;
}


