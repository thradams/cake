#pragma safety enable 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
    char *_Owner _Opt name;
};

void f(_View struct X x)
{
    if (x.name)
        printf(x.name);
}

int main() {
    struct X x = {0};
    x.name = strdup("a");
    f(x); /*not moved*/
    free(x.name);
}
