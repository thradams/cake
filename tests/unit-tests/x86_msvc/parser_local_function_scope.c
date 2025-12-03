/* Cake x86_msvc */

void f();
static int g;
static int __c0_s;
static void __c2_f()
{
    f();
    g++;
    __c0_s++;
    i++;
}

int main()
{
    int i;

    __c2_f;
}


