#include <stdio.h>
#include <stdlib.h>

_Noreturn void not_coming_back(void)
{
    puts("There's no coming back");
    exit(1);
    return;
}
