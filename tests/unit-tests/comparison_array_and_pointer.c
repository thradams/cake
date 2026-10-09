void f(void) {}

int main()
{
    char line[10] = { 0 };
    char* p = line;

    /* arrays and functions are compared as pointers */
    if (p == line) {}
    if (line == p) {}
    if (p < line) {}
    void (*pf)(void) = f;
    if (pf == f) {}

    //warning: comparison between pointer and non-pointer
    int i = 1;
    if (line == i) {} //lint 4
}
