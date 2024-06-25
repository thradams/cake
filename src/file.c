#pragma safety enable

#include <ownership.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
    char * _Owner _Opt s;
};
void destroy(struct X * obj_owner p) {free(p->s);}


void f2(){
    struct X* _Opt p = 0;

    {
        struct X x = {0};    
        p = &x;
        x.s = strdup("a");
        destroy(&x);
    }
    static_debug(p->s);
    printf("%s", p->s);
}

int main(){
    
}