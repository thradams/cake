/* Cake 0.14.05 x86_msvc */

void (*F(int a, int b))(void)
{
    return 0;
}
void (*(* PF)(int a, int b))(void) = F;

int main()
{
    PF(1, 2);
}
