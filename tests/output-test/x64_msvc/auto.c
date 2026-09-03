/* Cake 0.14.37 x64_msvc 2026-08-28 13:54:46 */

extern int func(void);
int (* p_func2)(void) = func;

static int (* p_func1)(void) = func;


int main()
{
    p_func1 = 0;
    p_func2 = 0;
}
