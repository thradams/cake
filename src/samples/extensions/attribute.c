/*
  GCC/Clang __attribute__ syntax is parsed in every -std mode.

  The attributes that change struct layout are honored:
    packed      - see attribute-packed-aligned
    aligned(n)  - see attribute-packed-aligned

  The others are accepted and ignored. Cake implements C23 [[...]]
  attributes; use those where a standard spelling exists.
*/

__attribute__((noreturn)) void die(void);
__attribute__((unused)) static int unused_var;

int f(int a) __attribute__((nonnull));

int main()
{
    return 0;
}
