/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

void *malloc(unsigned long size);

int main()
{
    struct X * p;

    p = (struct X *)malloc(1);
}
