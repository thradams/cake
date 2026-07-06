/* Cake 0.14.05 x86_msvc */

struct X {
    char * name;
};

void *malloc(int i);

int main()
{
    struct X * p;

    p = malloc(4);
    p = 0;
}
