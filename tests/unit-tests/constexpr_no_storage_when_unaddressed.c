/*
   github.com/thradams/cake/issues/415

   A scalar `constexpr` local has every use already replaced by its
   constant value at the use site (constant folding is independent of
   codegen), so declaring real storage for it and assigning into it is dead
   code -- UNLESS something takes its address, which needs a real object to
   point at. codegen now skips the declaration + initializer entirely for a
   scalar constexpr local whose address is never taken
   (declarator->address_taken, set in expressions.c's '&' handling), and
   keeps it when the address IS taken.

   This can't be checked by inspecting the generated C's structure from a
   -test-mode run (that only checks diagnostics), so it's a runtime check:
   correctness of every VALUE that would have come from `ch`/`n` is what
   actually matters, and a regression here would most likely show up as a
   compile error (an unaddressed variable's dropped declaration leaving a
   dangling reference) rather than a silently wrong value, so this doubles
   as confirming the deleted declaration doesn't break any real use.
*/

int printf(const char* fmt, ...);

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static void use(const int* p)
{
    CHECK(*p == 5);
}

int main(void)
{
    /* not address-taken: storage should be elided entirely */
    constexpr char ch = 'a';
    CHECK(ch == 'a');

    constexpr int n = 5;
    int y = n + 1;
    CHECK(y == 6);
    if (n)
    {
        CHECK(n == 5);
    }
    int arr[n];
    CHECK((int)(sizeof(arr) / sizeof(arr[0])) == 5);

    /* address-taken: storage must still exist */
    constexpr int addressed = 5;
    use(&addressed);

    return failures;
}
