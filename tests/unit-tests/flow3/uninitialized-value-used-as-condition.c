#pragma safety enable

/* a value used as it is -- a condition (if, loops, ?:, &&, ||, !) or an operand of unary - and + -- is checked for warning 30, like an arithmetic operand */

void if_condition(void)
{
    int i;
    if (i) {} //lint 30
}

void while_condition(void)
{
    int i;
    while (i) { break; } //lint 30
}

void not_operand(void)
{
    int i;
    int k = !i; //lint 30
    (void)k;
}

void conditional_operand(void)
{
    int i;
    int k = i ? 1 : 2; //lint 30
    (void)k;
}

void logical_operand(int a)
{
    int i;
    if (a && i) {} //lint 30
}

void negate_operand(void)
{
    int i;
    int k = -i; //lint 30
    (void)k;
}

void pointer_condition(void)
{
    int* p;
    if (p) {} //lint 30
}

/* the count belongs to the tested expression only: the inner condition is
   not a test of i, even though it runs inside i's narrowed branch */
void nested_other_condition(int a)
{
    int i;
    if (i) //lint 30
    {
        if (a + 1) {}
    }
}

void initialized(int a)
{
    int i = a;
    if (i) {}
    while (i) { break; }
    int k = !i + -i + (i ? 1 : 2);
    (void)k;
}
