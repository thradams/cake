/* Cake x64_msvc */

struct X {
    char * text;
};

void *malloc(unsigned long size);
void x_delete(struct X * p);

int main()
{
    struct X * p;

    p = malloc(8ULL);
    x_delete(p);
}
