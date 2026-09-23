/*
  In the generated code every struct declared under a pack value is
  wrapped in its own #pragma pack(push, n) / #pragma pack(pop), so the
  host compiler lays it out exactly as Cake did.

  The wrapper travels with the struct rather than with the original
  directive, because Cake emits only the structs a file uses, in
  dependency order rather than source order.

  Compile this and compare the output with the source.
*/

#pragma pack(push, 1)
typedef struct { char c; int i; } packed_t;
struct named { char c; int i; };
#pragma pack(pop)

struct outer { char c; struct named n; packed_t p; };

int main(void)
{
    struct outer o = {0};
    return o.c;
}
