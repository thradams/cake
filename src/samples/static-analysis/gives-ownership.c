#pragma safety enable

#include <string.h>
#include <stdlib.h>

const char * _Owner _Opt gives_ownership() {
    _Owner _Opt auto some_string = strdup("yours");
    return some_string;
}

int main(){
    _Owner _Opt auto s = gives_ownership();
    free(s);
}
