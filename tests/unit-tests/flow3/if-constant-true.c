/* the false path of if (1) is dead; static const keeps its value, so if (K) too */

static const int K = 1;
void f(const char* s);

void g(void)
{
    char* p;
    if (1) //lint 44
        p = "";
    f(p);
}

void h(void)
{
    char* p;
    if (K) //lint 44
        p = "";
    f(p);
}
