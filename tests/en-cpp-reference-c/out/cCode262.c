//en.cppreference.com/w/c/numeric/math/nan.html
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
 
int main(void)
{
    double f1 = nan("1");
    uint64_t f1n; memcpy(&f1n, &f1, sizeof f1);
    printf("nan(\"1\")   = %f (%" PRIx64 ")\n", f1, f1n);
 
    double f2 = nan("2");
    uint64_t f2n; memcpy(&f2n, &f2, sizeof f2);
    printf("nan(\"2\")   = %f (%" PRIx64 ")\n", f2, f2n);
 
    double f3 = nan("0xF");
    uint64_t f3n; memcpy(&f3n, &f3, sizeof f3);
    printf("nan(\"0xF\") = %f (%" PRIx64 ")\n", f3, f3n);
}