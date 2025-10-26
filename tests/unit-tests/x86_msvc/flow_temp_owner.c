/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void *malloc(unsigned long size);

int main()
{
    struct X * p;

    p = (struct X *)malloc(1);
}


