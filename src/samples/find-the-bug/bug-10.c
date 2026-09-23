#pragma safety enable

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct person {
    char* _Opt _Owner name;
};

void set(struct person* p, char* name) {
    free(p->name);
    char* _Opt _Owner temp = strdup(name);
    if (temp == NULL) return;
    p->name = temp;
    /*may leave this function with p->name uninitialized!*/
}

int main() {
    struct person p = {0};
    set(&p, "a");
    printf("%s", p.name);
    free(p.name);
    return 0;
}
