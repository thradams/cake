/*
   github.com/thradams/cake/issues/430

   A pointer to a VLA array (`int (*p)[n]`) has its pointee size known only
   at runtime. Codegen used to emit `p++`, `p--`, `++p`, `--p` and `p + 1`
   as literal C pointer arithmetic on the transpiled `int (*p)[]` -- a
   pointer to an INCOMPLETE array type, on which arithmetic isn't even legal
   C. Each case below is checked at runtime against the address a manual
   `(char*)p + n * sizeof(int)` computation would give, so a codegen
   regression that stops scaling by n (or scales by the wrong amount) fails
   here instead of only failing to compile.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int main()
{
    int n = 3;
    int buffer[3][3] = { 0 };
    int (*p)[n] = buffer;

    char* base = (char*)p;
    size_t row_size = (size_t)n * sizeof(int);

    /* postfix ++, used as a value: old pointer returned, then advanced */
    int (*old1)[n] = p++;
    CHECK((char*)old1 == base);
    CHECK((char*)p == base + row_size);

    /* postfix --, used as a value: old pointer returned, then stepped back */
    int (*old2)[n] = p--;
    CHECK((char*)old2 == base + row_size);
    CHECK((char*)p == base);

    /* prefix ++: value IS the new pointer */
    int (*new1)[n] = ++p;
    CHECK((char*)new1 == base + row_size);
    CHECK((char*)p == base + row_size);

    /* prefix --: value IS the new pointer */
    int (*new2)[n] = --p;
    CHECK((char*)new2 == base);
    CHECK((char*)p == base);

    /* binary +, discarded value (matches the issue's `p + 1;` repro) */
    p + 1;

    /* binary +, used as a value */
    int (*sum1)[n] = p + 2;
    CHECK((char*)sum1 == base + 2 * row_size);

    /* binary -, used as a value */
    p = p + 2;
    int (*sum2)[n] = p - 1;
    CHECK((char*)sum2 == base + row_size);

    /* postfix ++/-- as a bare discarded statement (no value needed) */
    p = buffer;
    p++;
    CHECK((char*)p == base + row_size);
    p--;
    CHECK((char*)p == base);

    return failures;
}
