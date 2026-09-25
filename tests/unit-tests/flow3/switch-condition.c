#pragma safety enable

void uninit_condition()
{
    int x;
    switch (x) //lint 30
    {
        case 1:
            break;
    }
}

void no_default_skips_body(int x)
{
    int y;
    switch (x)
    {
        case 1: y = 1; break;
        case 2: y = 2; break;
    }
    int z = y; //lint 30
    (void)z;
}

void with_default(int x)
{
    int y;
    switch (x)
    {
        case 1: y = 1; break;
        default: y = 2; break;
    }
    int z = y; /* ok */
    (void)z;
}

int case_narrows(int x)
{
    switch (x)
    {
        case 0:
            return 10 / x; //lint 36
    }
    return 0;
}

struct kind_holder { int kind; };

void case_narrows_member(const struct kind_holder* p)
{
    switch (p->kind)
    {
        case 3:
            compile_assert(p->kind == 3);
            break;
    }
}
