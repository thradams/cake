#pragma safety enable

/* the operand of ++ and -- is read: checked for warning 30, like an arithmetic operand */

void postfix_increment(void)
{
    int i;
    i++; //lint 30
}

void prefix_decrement(void)
{
    int i;
    --i; //lint 30
}

void initialized(int a)
{
    int i = a;
    i++;
    --i;
}
