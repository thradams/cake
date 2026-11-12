#pragma safety enable

/*
    The condition of a do-while runs after every iteration: a problem that
    exists only from the second iteration on is reported, once.
*/

int get(void);

void null_only_from_the_second_iteration(void)
{
    int a = 1;
    int* p = &a;
    int* next = &a;
    do
    {
        p = next;
        next = 0;
    }
    while (*p > 0 && get()); //lint 33
}

void null_on_the_first_iteration(void)
{
    int* p = 0;
    do
    {
    }
    while (*p > 0 && get()); //lint 33
}

void never_null(void)
{
    int a = 1;
    int* p = &a;
    do
    {
        a++;
    }
    while (*p > 0 && get());
}
