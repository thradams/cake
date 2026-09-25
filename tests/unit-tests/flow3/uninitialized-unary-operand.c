#pragma safety enable

/* an operand of unary - and + is checked for warning 30, like an arithmetic operand */

void negate_operand(void)
{
    int i;
    int k = -i; //lint 30
    (void)k;
}

void initialized(int a)
{
    int i = a;
    int k = -i + +i;
    (void)k;
}
