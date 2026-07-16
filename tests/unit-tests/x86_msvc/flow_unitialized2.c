/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

void f_const(char * s);
void f_non_const(char * s);

int main(void)
{
    char buffer[20];

    f_const(buffer);
    f_non_const(buffer);
}
