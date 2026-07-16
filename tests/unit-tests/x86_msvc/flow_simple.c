/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

void *malloc(int i);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(4);
    free(p);
}
