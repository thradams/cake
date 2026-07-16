/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

void (*F(int a, int b))(void)
{
    return 0;
}
void (*(* PF)(int a, int b))(void) = F;

int main()
{
    PF(1, 2);
}
