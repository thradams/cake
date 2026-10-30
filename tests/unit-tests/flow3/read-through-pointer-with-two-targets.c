#pragma safety enable

/* a read through a pointer with two targets sees only the value of its path */

void comparison(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    compile_assert(*p == 0);
}

void postfix_increment(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    (*p)++;
    compile_assert(*p == 1);
}

void arithmetic(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    compile_assert(*p + 1 == 1);
}

void array_index(int c)
{
    int arr[2] = { 0 };
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    arr[*p] = 1;
}

void comma(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    compile_assert((0, *p) == 0);
}

void compile_assert_value(int c)
{
    int a = 0;
    int b = 0;
    int* p = &a;
    if (c) { p = &b; }
    *p = 1;
    compile_assert(*p);
}
