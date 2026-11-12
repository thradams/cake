#pragma safety enable

/*
    switch (*p) with p pointing to a or b: in `case N` the object p points to
    is N, on the path of each target. The other target keeps its values.
*/

int get(void);

void switch_through_pointer_with_two_targets(int c)
{
    int a = get();
    int b = get();
    int* p = c ? &a : &b;
    switch (*p)
    {
    case 1:
        compile_assert(*p == 1);
        compile_assert(a == 1); //lint 67
        compile_assert(b == 1); //lint 67
        break;
    case 2:
        compile_assert(*p == 2);
        break;
    default:
        break;
    }
}

void switch_through_pointer_with_one_target(void)
{
    int a = get();
    int* p = &a;
    switch (*p)
    {
    case 1:
        compile_assert(*p == 1);
        compile_assert(a == 1);
        break;
    default:
        break;
    }
}

void switch_on_variable(void)
{
    int a = get();
    switch (a)
    {
    case 3:
        compile_assert(a == 3);
        break;
    default:
        break;
    }
}
