/* Cake 0.12.26 x86_msvc */

extern int func(void);

int (* p_func2)(void) = func;
static int (* p_func1)(void) = func;


int main()
{
    p_func1 = 0;
    p_func2 = 0;
}


