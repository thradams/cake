//en.cppreference.com/w/c/language/main_function.html
#include <stdio.h>
 
int main(int argc, char *argv[])
{
    printf("argc = %d\n", argc);
    for (int ndx = 0; ndx != argc; ++ndx)
        printf("argv[%d] --> %s\n", ndx, argv[ndx]);
    printf("argv[argc] = %p\n", (void*)argv[argc]);
}