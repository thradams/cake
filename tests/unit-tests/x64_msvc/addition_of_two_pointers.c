/* Cake x64_msvc */

int main()
{
    int a;
    int b;
    int * p1;
    int * p2;
    int * p3;

    a = 10;
    b = 20;
    p1 = &a;
    p2 = &b;
    p3 = p1 + p2;
    return 0;
}
