/* the first check of the for condition is known: the loop runs at least once */

void f(const char* s);

void first_check_true(void)
{
    char* p;
    for (int h = 0; h < 1; h++) p = "";
    f(p);
}

void first_check_unknown(int n)
{
    char* p;
    for (int h = 0; h < n; h++) p = "";
    f(p); //lint 30
}
