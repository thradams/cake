//en.cppreference.com/w/c/types/limits.html
#include <float.h>
#include <math.h>
#include <stdio.h>
 
int main(void)
{
    printf("DECIMAL_DIG     = %d\n", DECIMAL_DIG);
    printf("FLT_DECIMAL_DIG = %d\n", FLT_DECIMAL_DIG);
    printf("FLT_RADIX       = %d\n", FLT_RADIX);
    printf("FLT_MIN         = %e\n", FLT_MIN);
    printf("FLT_MAX         = %e\n", FLT_MAX);
    printf("FLT_EPSILON     = %e\n", FLT_EPSILON);
    printf("FLT_DIG         = %d\n", FLT_DIG);
    printf("FLT_MANT_DIG    = %d\n", FLT_MANT_DIG);
    printf("FLT_MIN_EXP     = %d\n", FLT_MIN_EXP);
    printf("FLT_MIN_10_EXP  = %d\n", FLT_MIN_10_EXP);
    printf("FLT_MAX_EXP     = %d\n", FLT_MAX_EXP);
    printf("FLT_MAX_10_EXP  = %d\n", FLT_MAX_10_EXP);
    printf("FLT_ROUNDS      = %d\n", FLT_ROUNDS);
    printf("FLT_EVAL_METHOD = %d\n", FLT_EVAL_METHOD);
    printf("FLT_HAS_SUBNORM = %d\n", FLT_HAS_SUBNORM);
}