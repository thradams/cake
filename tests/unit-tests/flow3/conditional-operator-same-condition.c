#pragma safety enable

int two_conditionals(int c)
{
    int a[4] = { 0 };
    int i = c ? 1 : 4;
    int j = c ? 0 : -1;
    return a[i + j]; /* 1 or 3 */
}

int if_then_conditional(int c)
{
    int a[4] = { 0 };
    int i;
    if (c) { i = 1; } else { i = 4; }
    int j = c ? 0 : -1;
    return a[i + j]; /* 1 or 3 */
}

int conditional_arm_reads(int c)
{
    int a[4] = { 0 };
    int i = c ? 1 : 4;
    return c ? a[i] : a[i - 1]; /* 1 or 3 */
}

int other_condition(int c, int d)
{
    int a[4] = { 0 };
    int i = c ? 1 : 4;
    int j = d ? 0 : -1;
    return a[i + j]; //lint 70
}
