#pragma safety enable

/* a member or element used as a condition is read: checked for warning 30 */

struct s { int x; };

void dot_condition(void)
{
    struct s s;
    if (s.x) {} //lint 30
}

void arrow_condition(struct s* p)
{
    p->x = 1;
    if (p->x) {}
}

void element_condition(void)
{
    int a[2];
    if (a[0]) {} //lint 30
}

void initialized(void)
{
    struct s s = { 0 };
    int a[2] = { 0 };
    if (s.x) {}
    if (a[0]) {}
}
