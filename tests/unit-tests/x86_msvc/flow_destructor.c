/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};

struct Y {
    struct X  x;
};


void x_destroy(struct X * p);

void y_destroy(struct Y * p)
{
    x_destroy(&p->x);
}


