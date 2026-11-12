#pragma safety enable

/*
    A write through a pointer that may point to several objects writes one of
    them: each target may hold the new value or keep its old one.
*/

void targets_from_two_ifs(int c1, int c2)
{
    int x = 0;
    int a = 0;
    int b = 0;
    int* p = &x;
    if (c1)
        p = &a;
    if (c2)
        p = &b;
    *p = 1;
    compile_assert(x == 0 || x == 1);
    compile_assert(a == 0 || a == 1);
    compile_assert(b == 0 || b == 1);
    compile_assert(x == 1); //lint 67
    compile_assert(a == 1); //lint 67
    compile_assert(b == 1); //lint 67
}

void null_target_is_kept(int c1, int c2)
{
    int i = 0;
    int* x = 0;
    int* a = &i;
    int* b = &i;
    int** p = &x;
    if (c1)
        p = &a;
    if (c2)
        p = &b;
    *p = &i;
    compile_assert(x != 0); //lint 67 67
}
