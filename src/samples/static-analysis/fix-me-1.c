#pragma safety disable

#include <stdlib.h>
#include <string.h>

struct X {
    char * text;
};

int main() {
    struct X x = {};
    x.text = strdup("a");
}
