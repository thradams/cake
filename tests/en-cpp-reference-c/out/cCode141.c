//en.cppreference.com/w/c/language/return.html
#include <stdio.h>
 
void fa(int i)
{
    if (i == 2)
       return;
    printf("fa():   %d\n", i);
} // implied return;
 
int fb(int i)
{
    if (i > 4)
       return 4;
    printf("fb():   %d\n", i);
    return 2;
}
 
int main(void)
{
    fa(2);
    fa(1);
    int i = fb(5);   // the return value 4 used to initializes i
    i = fb(i);       // the return value 2 used as rhs of assignment
    printf("main(): %d\n", i);
}