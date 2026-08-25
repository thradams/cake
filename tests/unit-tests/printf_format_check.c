/*
   check_fmt / check_printf_conversion (src/expressions.c), warning 86.

   A `//lint 86` here asserts the warning FIRES on that line -- an unused
   suppression is reported as warning 59, so both directions are locked in.
*/

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <wchar.h>

void valid(int i, unsigned u, long l, long long ll, size_t z, ptrdiff_t t,
    intmax_t j, double d, long double ld, char c, const char* s,
    const wchar_t* ws, void* pv, int* pi, unsigned char uc, char arr[10])
{
    printf("%d %i %u %o %x %X\n", i, i, u, u, u, u);
    printf("%ld %lld %zu %td %jd %hhd\n", l, ll, z, t, j, uc);
    printf("%f %F %e %E %g %G %a %A %Lf\n", d, d, d, d, d, d, d, d, ld);
    printf("%c %s %lc %ls\n", c, s, (wint_t)c, ws);
    printf("%p %n\n", pv, pi);
    printf("%5d %-5d %+d % d %#x %05d %*d %.*f %10.4s\n",
        i, i, i, i, u, i, i, i, i, d, s);
    printf("100%% done\n");

    /* An array argument decays: char[10] is a valid %s AND a valid %p. */
    printf("%s %p\n", arr, arr);

    /* C23 length modifiers. */
    printf("%w32d %wf16d\n", (int_least32_t)i, (int_fast16_t)i);
}

void wrong_type(int i, double d, const char* s, int* pi)
{
    printf("%d\n", d);      //lint 86 int conversion, floating argument
    printf("%f\n", i);      //lint 86 floating conversion, integer argument
    printf("%s\n", i);      //lint 86 not a char *
    printf("%c\n", d);      //lint 86 not an integer
    printf("%p\n", i);      //lint 86 not a pointer
    printf("%n\n", i);      //lint 86 not a pointer
    printf("%ls\n", s);     //lint 86 char * where wchar_t * is expected
    printf("%lld\n", d);    //lint 86 message echoes the length modifier
    printf("%w32d\n", d);   //lint 86 wN is checked like the plain conversion
    printf("%*d\n", d, i);  //lint 86 field width is not an integer
    printf("%.*f\n", d, d); //lint 86 field precision is not an integer
    printf("%d\n", pi);     //lint 86 not an integer
}

/*
   Both sides being integers is not enough -- printf reads a fixed number of
   bytes off the va_list, so the length modifier has to name the TYPE that is
   passed. The type is what is compared, not the width it happens to have on
   the target being compiled for: "%d" with a long is reported on every
   target, including the ones where long and int are both 32 bits, because
   the same source is wrong on the next target. A typedef that owns a
   modifier (size_t, ptrdiff_t) is held to it for the same reason.
*/
void wrong_width(long long ll, long l, int i, size_t z, ptrdiff_t t,
    double d, long double ld, char c, short sh, unsigned u)
{
    /* Correct: the modifier matches what arrives. */
    printf("%lld %ld %d %zu %td\n", ll, l, i, z, t);
    printf("%zd %jd %ju\n", z, (intmax_t)ll, (uintmax_t)ll);
    printf("%Lf %f\n", ld, d);
    /* char and short arrive promoted to int, so all of these are right. */
    printf("%d %d %hhd %hd %c\n", c, sh, c, sh, c);
    /* Signedness alone is not reported: %x with an int is idiomatic. */
    printf("%x %u %d\n", i, i, u);

    printf("%d\n", ll);   //lint 86 int is not long long
    printf("%d\n", l);    //lint 86 long is not int, whatever its width here
    printf("%d\n", z);    //lint 86 size_t
    printf("%d\n", t);    //lint 86 ptrdiff_t
    printf("%lld\n", i);  //lint 86 long long is not int
    printf("%f\n", ld);   //lint 86 long double needs %Lf
    printf("%Lf\n", d);   //lint 86 double is not a long double
}

/*
   A typedef is erased from the type, so the suggestion goes back to the
   declaration to recover it: size_t earns "%zu", not the "%lu" its resolved
   type would suggest, and the fixed-width types are pointed at their
   <inttypes.h> macro, which is the only portable answer for them.
*/
typedef size_t my_size;      /* a typedef of a typedef still finds size_t */
struct counters { size_t n; };

void suggestions(size_t z, ptrdiff_t t, uint64_t u64, int32_t i32, unsigned long ul,
    my_size m, const struct counters* p)
{
    printf("%d\n", z);          //lint 86 suggests %zu
    printf("%d\n", t);          //lint 86 suggests %td
    printf("%d\n", u64);        //lint 86 suggests PRIu64
    printf("%x\n", u64);        //lint 86 suggests PRIx64, keeping the conversion
    printf("%s\n", i32);        //lint 86 suggests PRId32
    printf("%d\n", ul);         //lint 86 no typedef: suggests %lu

    printf("%d\n", m);          //lint 86 my_size -> size_t -> %zu
    printf("%d\n", p->n);       //lint 86 the member's own declaration is read

    printf("%zu %td %zu %zu\n", z, t, m, p->n);
    printf("%" PRIu64 " %" PRId32 "\n", u64, i32);
    printf("%lu\n", ul);
}

void wrong_count(int i, const char* s)
{
    printf("%d %d\n", i);   //lint 86 too few
    printf("%d\n", i, i);   //lint 86 too many
    printf("%d\n");         //lint 86 too few
    printf("%*d\n");        //lint 86 too few (the '*' takes one too)
    printf("%");            //lint 86 incomplete specifier
    printf("%s %d\n", s, s);//lint 86 second argument is not an integer
}
