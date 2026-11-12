#pragma safety enable

/*
    A pointer that may point to a or to b (from a ?:) writes one of them: each
    target may hold the new value or keep its old one.
*/


void conditional_operator_targets(int c)
{
    int a = 0;
    int b = 0;
    int* pa = &a;
    int* pb = &b;
    int* q = c ? pa : pb;
    *q = 1;
    compile_assert(a == 0 || a == 1);
    compile_assert(b == 0 || b == 1);
    compile_assert(a == 1); //lint 67
    compile_assert(b == 1); //lint 67
}

void conditional_operator_with_addresses(int c)
{
    int a = 0;
    int b = 0;
    int* q = c ? &a : &b;
    *q = 1;
    compile_assert(a == 1); //lint 67
    compile_assert(b == 1); //lint 67
}

void conditional_operator_read_back(int c)
{
    int a = 1;
    int b = 2;
    int* q = c ? &a : &b;
    compile_assert(*q == 1 || *q == 2);
    compile_assert(*q == 1); //lint 67
}

void one_target_keeps_the_overwrite(void)
{
    int a = 0;
    int* p = &a;
    *p = 1;
    compile_assert(a == 1);
}
