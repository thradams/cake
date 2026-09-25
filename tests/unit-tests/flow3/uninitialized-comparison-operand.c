#pragma safety enable

/* an uninitialized operand of a relational or equality operator is reported, as it already was for arithmetic operators */

void relational(int n)
{
    int i;
    if (i < n) {} //lint 30
}

void relational_right(int n)
{
    int i;
    if (n > i) {} //lint 30
}

void equality(int n)
{
    int i;
    if (i == n) {} //lint 30
}

void in_for_condition(int n)
{
    int i;
    for (; i < n;) { return; } //lint 30
}

void initialized(int n)
{
    int i = 0;
    if (i < n) {}
    if (i != n) {}
}
