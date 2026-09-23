/*
  #pragma pack(n) limits member alignment to at most n bytes for the
  structs declared while it is in effect. It takes part in sizeof,
  _Alignof and offsetof, and is carried into the generated code so the
  host compiler lays the struct out identically.
*/

#include <stddef.h>

#pragma pack(push, 1)
struct packed { char c; int i; };
#pragma pack(pop)

struct normal { char c; int i; };

static_assert(sizeof(struct packed) == 5);
static_assert(offsetof(struct packed, i) == 1);
static_assert(_Alignof(struct packed) == 1);
static_assert(sizeof(struct normal) == 8);

int main()
{
    struct packed p = {0};
    return p.c;
}
