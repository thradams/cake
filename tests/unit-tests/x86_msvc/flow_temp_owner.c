/* Cake 0.14.05 x86_msvc */

struct X {
    char * name;
};

void *malloc(unsigned long size);

int main()
{
    struct X * p;

    p = (struct X *)malloc(1);
}
