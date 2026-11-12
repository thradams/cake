#pragma safety enable

/*
    A member (or element) tested through a pointer that may point to two
    objects: an arm is unreachable only when no target takes it. One target
    that cannot take the arm does not close it for the other.
*/

struct S { int m; };

void arrow_two_targets(int c)
{
    struct S a = { 0 };
    struct S b = { 1 };
    struct S* p = c ? &a : &b;
    int n = 0;
    if (p->m)
    {
        n = 1;
    }
    else
    {
        n = 2;
    }
    compile_assert(n == 1 || n == 2);
    compile_assert(n == 1); //lint 67
    compile_assert(n == 2); //lint 67
}

void dot_two_targets(int c)
{
    struct S a = { 0 };
    struct S b = { 1 };
    struct S* p = c ? &a : &b;
    int n = 0;
    if ((*p).m)
    {
        n = 1;
    }
    else
    {
        n = 2;
    }
    compile_assert(n == 1 || n == 2);
    compile_assert(n == 1); //lint 67
}

void subscript_two_targets(int c)
{
    int a = 0;
    int b = 1;
    int* p = c ? &a : &b;
    int n = 0;
    if (p[0])
    {
        n = 1;
    }
    else
    {
        n = 2;
    }
    compile_assert(n == 1 || n == 2);
    compile_assert(n == 1); //lint 67
}

void array_element_two_targets(int c)
{
    struct A { int v[2]; };
    struct A a = { { 0, 0 } };
    struct A b = { { 1, 1 } };
    struct A* p = c ? &a : &b;
    int n = 0;
    if (p->v[1])
    {
        n = 1;
    }
    else
    {
        n = 2;
    }
    compile_assert(n == 1 || n == 2);
    compile_assert(n == 1); //lint 67
}

void arrow_one_target_keeps_folding(void)
{
    struct S a = { 0 };
    struct S* p = &a;
    int n = 0;
    if (p->m)
    {
        n = 1; //lint 68
    }
    compile_assert(n == 0);
}
