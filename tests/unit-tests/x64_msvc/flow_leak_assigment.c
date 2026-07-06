/* Cake x64_msvc */

struct X {
    char * name;
};

void *malloc(int i);

int main()
{
    struct X * p;

    p = malloc(8ULL);
    p = 0;
}
