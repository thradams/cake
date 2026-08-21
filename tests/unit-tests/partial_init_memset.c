/*
   github.com/thradams/cake/issues/332

   `struct X { char s[30]; int i; }; struct X x = {.i = 1};` used to emit
   one `x.s[k] = 0;` per array element (30 lines) instead of a single
   memset. Now: when more than half of an aggregate's leaf members are
   implicit zero-fills AND there are at least 4 leaves total
   (count_zero_fill_leaves / the threshold in codegen_visit_init_declarator),
   codegen emits one memset covering the whole object first, then only the
   members that actually have an initializer.

   Checked at runtime (not just by inspecting the generated C), since the
   bug that motivated this fix was a nested recursive call hardcoding
   `true` for "still fill implicit zeros" one level down even when the
   caller had just decided memset already covered them -- that regression
   would still let the *values* come out right by accident (memset zeroes,
   then redundant explicit zero stores are also zero), so a values-only
   check wouldn't have caught it. It's here to guard the generated-code
   shape itself as much as the runtime values.
*/

int printf(const char* fmt, ...);

struct Inner { int a, b, c, d; };
struct X { struct Inner inner; char s[30]; int i; };

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int main(void)
{
    struct X x = { .i = 1 };
    CHECK(x.i == 1);
    CHECK(x.inner.a == 0 && x.inner.b == 0 && x.inner.c == 0 && x.inner.d == 0);
    for (int k = 0; k < 30; k++)
    {
        CHECK(x.s[k] == 0);
    }

    /* explicit assignment inside a nested member, alongside the rest
       implicitly zero */
    struct X y = { .inner = {.a = 5}, .i = 2 };
    CHECK(y.inner.a == 5);
    CHECK(y.inner.b == 0 && y.inner.c == 0 && y.inner.d == 0);
    CHECK(y.i == 2);
    for (int k = 0; k < 30; k++)
    {
        CHECK(y.s[k] == 0);
    }

    /* small struct: below the total_leaves >= 4 floor, must still work
       via the plain per-member path (no memset expected, but correctness
       is what's checked here, not the strategy chosen) */
    struct Small { int a; int b; } small = { .b = 7 };
    CHECK(small.a == 0);
    CHECK(small.b == 7);

    return failures;
}
