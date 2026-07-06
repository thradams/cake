/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("DECIMAL_DIG     = %d\n", 21);
    printf("FLT_DECIMAL_DIG = %d\n", 9);
    printf("FLT_RADIX       = %d\n", 2);
    printf("FLT_MIN         = %e\n", 1.1754943508222875e-38f);
    printf("FLT_MAX         = %e\n", 3.4028234663852886e+38f);
    printf("FLT_EPSILON     = %e\n", 1.1920928955078125e-07f);
    printf("FLT_DIG         = %d\n", 6);
    printf("FLT_MANT_DIG    = %d\n", 24);
    printf("FLT_MIN_EXP     = %d\n", -125);
    printf("FLT_MIN_10_EXP  = %d\n", -37);
    printf("FLT_MAX_EXP     = %d\n", 128);
    printf("FLT_MAX_10_EXP  = %d\n", 38);
    printf("FLT_ROUNDS      = %d\n", 1);
    printf("FLT_EVAL_METHOD = %d\n", 0);
    printf("FLT_HAS_SUBNORM = %d\n", 1);
}


