#pragma safety enable

/*
    A pointer carried by a loop may point to a or to b: a write through it
    reaches one of them, the other keeps its value.
*/

int get(void);

void while_loop_carried_pointer(void)
{
    int a = 0;
    int b = 0;
    int* p = &a;
    while (get())
    {
        p = &b;
    }
    *p = 1;
    compile_assert(a == 0 || a == 1);
    compile_assert(b == 0 || b == 1);
    compile_assert(a == 1); //lint 67
    compile_assert(b == 1); //lint 67
}

void for_loop_carried_pointer(void)
{
    int a = 0;
    int b = 0;
    int* p = &a;
    for (int i = 0; i < 3; i++)
    {
        p = &b;
    }
    *p = 1;
    compile_assert(a == 0 || a == 1);
    compile_assert(b == 0 || b == 1);
}

void write_inside_the_loop(void)
{
    int a = 0;
    int b = 0;
    int* p = &a;
    while (get())
    {
        *p = 1;
        compile_assert(b == 0 || b == 1);
        p = &b;
    }
}
