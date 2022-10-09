

/*
  OBS:
  This is very EXPERIMENTAL
*/

#include <annotations.h>

void* malloc(int i) {
    _add_attr(return, MUST_FREE);
    return 0;
}

void free(void* p) {
    static_assert(_has_attr(p, MUST_FREE));    
    _del_attr(p, MUST_FREE);
    _add_attr(p, UNINITIALIZED);
}

void* move(void* p)
{
    _del_attr(p, MUST_FREE);
    _add_attr(p, UNINITIALIZED);
    _add_attr(return, MUST_FREE);
    return p;
}

int main()
{
    int* p = malloc(10);

    int* p2;

    p2 = move(p);

    static_assert(!_has_attr(p, UNINITIALIZED));

    free(p2);

    free(p);
}
