/* Cake 0.14.05 x86_msvc */

void (*f(int i))(void)
{
    i = 1;
    return 0;
}

int main()
{
    f(1);
}
