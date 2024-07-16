//en.cppreference.com/w/c/program/getenv.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    const char *name = "PATH";
    const char *env_p = getenv(name);
    if (env_p)
        printf("Your %s is %s\n", name, env_p);
}