
static const int K = 1;

void f(const char* s);

void good(void)
{
    char* p;
    if (K) //lint 44
        p = "";
    f(p); /* expected: no warning */
}
