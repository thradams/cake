#pragma safety enable
#pragma cake diagnostic warning 85

void g(int i);
struct S { int x; };

void dead_branch_has_no_null_dereference(void)
{
    struct S* p = 0;
    if (p != 0) //lint 85
        g(p->x);
}

void known_comparison_outside_condition(void)
{
    struct S* p = 0;
    int b = (p != 0); //lint 85
    g(b);
}

void loop_condition_is_not_known(int n)
{
    int i = 0;
    while (i < n)
        i++;
    for (int k = 0; k < 10; k++)
        g(k);
}
