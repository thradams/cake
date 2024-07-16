//en.cppreference.com/w/c/language/static_storage_duration.html
#include <stdio.h>
 
void f (void)
{
    static int count = 0;   // static variable   
    int i = 0;              // automatic variable
    printf("%d %d\n", i++, count++);
}
 
int main(void)
{
    for (int ndx=0; ndx<10; ++ndx)
        f();
}