#pragma safety enable

#include <stdlib.h>

struct Obj2 {
    int i;
};

struct Obj {
    int i;
    struct Obj2* _Opt _Owner obj2;
};

void main() 
{
    struct Obj* _Opt _Owner o = calloc(1, sizeof *o);
    if (o) {
        o->obj2 = calloc(1, sizeof *o->obj2);
        if (o->obj2) {
            struct Obj* _Opt p = o;
            free(o->obj2);
            o->obj2 = nullptr;
            p->obj2->i = 1; //warning: object is possibly null 
        }
        free(o);
    }
}
