/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * text;
};

void f(struct X * p1, struct X ** p2)
{
    *p2 = p1;
}

void *malloc(unsigned long size);
void free(void * ptr);

int main()
{
    struct X * p1;

    p1 = malloc(4);
    if (p1)
    {
        struct X * p2;

        p1->text = 0;
        p2 = 0;
        f(p1, &p2);
        free(p2->text);
        free(p2);
    }
}
