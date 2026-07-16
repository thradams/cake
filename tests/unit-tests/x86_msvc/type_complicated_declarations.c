/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

void (*f(int i))(void)
{
    i = 1;
    return 0;
}

int main()
{
    f(1);
}
