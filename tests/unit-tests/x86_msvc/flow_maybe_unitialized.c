/* Cake 0.14.05 x86_msvc */

struct X {
    char * text;
};

void *malloc(unsigned long size);
void x_delete(struct X * p);

int main()
{
    struct X * p;

    p = malloc(4);
    x_delete(p);
}
