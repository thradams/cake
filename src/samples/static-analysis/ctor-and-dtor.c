#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
    char * _Owner _Opt text;
};

int init(_Out struct X *p)
{
    //comment and see what happens
    p->text = strdup("a");
}

void destructor(_Dtor struct X *p){
    //comment and see what happens
    free(p->text);
}

int main() {
    struct X x;
    init(&x);
    destructor(&x);
}
