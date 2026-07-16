/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

int add_nums_C23(int count, ...)
{
    int result;
    __builtin_va_list args;

    result = 0;
    __builtin_c23_va_start(args);
    {
        int i;

        i = 0;
        for (; i < count; ++i)
        {
            result += __builtin_va_arg(args, int);
        }
    }
    __builtin_va_end(args);
    return result;
}

int main(void)
{
    add_nums_C23(4, 25, 25, 50, 50);
}
