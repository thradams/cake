
#include <annotations.h>

void* malloc(int i) {
    _add_attr(return, MUST_FREE);
    return 0;
}

int main()
{
    struct x* px = malloc(sizeof(struct x));
    _add_attr(px, MUST_DESTROY);
}


