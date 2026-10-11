/* N3884 example with constexpr and static objects: automatic objects of the enclosing function, even constexpr, cannot be used */

#include <stdio.h>

int main()
{
    constexpr int i = 1;
    static constexpr int j = 0;
    static double array[2] = { 2.0, 3.0 };
    static void local()
    {
        printf("%d %d\n", i, j);  //lint 1870
        &i;                       //lint 1870
        &j;                       /* valid */
        static double* p = array; /* valid, using an address constant */
        double* q = &array[0];    /* valid, same, but q is automatic */
        double* r = &array[j];    /* valid, same */
        double* s = &array[i];    //lint 1870
        double brray[2] = { 4.0, 5.0 };
        double* t = &brray[0];    /* valid, both automatic */
        double* u = &brray[j];    /* valid, same */
        double* v = &brray[i-1];  //lint 1870
    }
}
