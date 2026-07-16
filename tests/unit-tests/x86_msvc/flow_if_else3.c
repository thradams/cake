/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * text;
};

struct X *make();
void free(void * p);
char *strdup(char * s);

void f(int condition)
{
    struct X * p;

    p = 0;
    if (condition)
    {
        p = make();
    }
    else
    {
        p = make();
    }
    free(p->text);
    p->text = strdup("c");
    free(p->text);
    free(p);
}
