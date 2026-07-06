/* Cake x86_msvc */

int f(char c)
{
    return c;
}


int main(void)
{
    int n;
    int * p;
    int (* fp)(char);
    int a[3];
    int * beg;
    int * end;

    n = 1;
    p = &n;
    fp = &f;
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    beg = a;
    end = &a[3];
}


