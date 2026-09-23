#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
    char *_Owner _Opt name;
};

void x_destroy(struct X x)
{
    free(x.name);
}

int main() {
    struct X x = {0};
    x.name = strdup("a");
    x_destroy(x);
}
