//en.cppreference.com/w/c/language/behavior.html
#include <stdio.h>
 
int fermat()
{
    const int MAX = 1000;
    // Endless loop with no side effects is UB
    for (int a = 1, b = 1, c = 1; 1;)
    {
        if (((a * a * a) == ((b * b * b) + (c * c * c))))
            return 1;
        ++a;
        if (a > MAX)
        {
            a = 1;
            ++b;
        }
        if (b > MAX)
        {
            b = 1;
            ++c;
        }
        if (c > MAX)
            c = 1;
    }
    return 0;
}
 
int main(void)
{
    if (fermat())
        puts("Fermat's Last Theorem has been disproved.");
    else
        puts("Fermat's Last Theorem has not been disproved.");
}