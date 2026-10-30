#pragma safety enable

#include <string.h>
#include <stdlib.h>

const char * _Owner gives_ownership() {
    _Owner auto some_string = strdup("yours");
    return some_string;
}

int main(){
    _Owner auto s = gives_ownership();
    free(s);
}
