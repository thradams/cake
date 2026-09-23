/*
  The GCC/Clang attributes that change struct layout are honored the
  same way #pragma pack is: they take part in sizeof, _Alignof and
  offsetof, and are carried into the generated code.

  - packed on a struct caps every member at alignment 1
  - packed on a single member gives only that member alignment 1
  - aligned(n) on a struct raises its alignment to at least n
  - aligned(n) on a member behaves as _Alignas(n)
*/

#include <stddef.h>

struct __attribute__((packed)) header { char tag; int length; };
struct record { char c; int i __attribute__((packed)); short s; };
struct __attribute__((aligned(16))) block { char c; };

static_assert(sizeof(struct header) == 5);
static_assert(_Alignof(struct header) == 1);
static_assert(offsetof(struct record, s) == 6);
static_assert(sizeof(struct record) == 8);
static_assert(sizeof(struct block) == 16);

int main()
{
    struct header h = {0};
    return h.tag;
}
