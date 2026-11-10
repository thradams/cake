void f(int a[3]);
void g(int a[static 3]);

void h(int c)
{
    int b[5] = {0};
    f(b + 2);
    f(b + 3); //lint 103
    int* p = b + 3;
    g(p); //lint 103
    int* q = c ? b : b + 4;
    f(q); //lint 103 /* b + 4 path only */
    const char* s = c ? "abc" : "a";
    void k(const char s[3]);
    k(s); //lint 103 /* "a" path only */
}
