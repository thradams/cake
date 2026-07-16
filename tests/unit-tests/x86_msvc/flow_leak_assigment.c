/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

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
