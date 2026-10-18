/* static without const can change: both paths of if (N) are possible */

static int N = 1;
void f(const char* s);

void g(void)
{
    char* p;
    if (N)
        p = "";
    f(p); //lint 30
}
