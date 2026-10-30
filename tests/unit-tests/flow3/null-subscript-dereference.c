#pragma safety enable

void g(int i);

void null_subscript_as_argument(void)
{
    char* p = 0;
    g(p[0]); //lint 33
}

void checked_subscript(const char* _Opt p)
{
    if (p)
        g(p[0]);
}
