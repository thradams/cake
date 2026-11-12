#pragma safety enable

/*
    An assignment used as a condition narrows the object it writes:
    `if ((p = f()))` makes p non-null in the true arm and null in the false one.
*/

int* f(void);
int get(void);

void assignment_in_if(void)
{
    int* p = 0;
    if ((p = f()))
    {
        *p = 1;
    }
}

void assignment_in_if_false_arm(void)
{
    int* p = 0;
    if (!(p = f()))
    {
        *p = 1; //lint 33
    }
}

void assignment_compared_with_null(void)
{
    int* p = 0;
    if ((p = f()) != 0)
    {
        *p = 1;
    }
}

void assignment_in_while(void)
{
    int* p = 0;
    while ((p = f()))
    {
        *p = 1;
    }
}

void assignment_in_and(void)
{
    int* p = 0;
    if ((p = f()) && get())
    {
        *p = 1;
    }
}

void without_the_test_it_still_warns(void)
{
    int* p = 0;
    p = f();
    *p = 1; //lint 33
}
