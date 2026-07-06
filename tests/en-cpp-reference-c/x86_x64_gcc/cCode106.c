/* Cake x86_x64_gcc */

int a = 1;
extern int printf(char * __format, ...);

void f(void)
{
    printf("from function f()\n");
}


static void g(void);

int main(void)
{
    f();
    g();
    return 0;
}



static void g(void)
{
    printf("from function g()\n");
}
