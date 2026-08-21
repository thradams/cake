/*
/*
 * 'void' cannot be used as a function parameter except for '(void)'.
 */

int compute_value(int left_value, void, int right_value) //lint 1990
{
    return left_value + right_value;
}