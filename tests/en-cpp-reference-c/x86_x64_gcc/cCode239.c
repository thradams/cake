/* Cake x86_x64_gcc */

extern int __fpclassifyf(float __value);
extern int __fpclassify(double __value);
extern int __fpclassifyl(long double __value);

char *show_classification(double x)
{
    /*switch*/
    {
        int __v0 = (0 ? __fpclassifyf(x) : 1 ? __fpclassify(x) : __fpclassifyl(x));
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 0) goto __L2; /*case 0*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 3) goto __L4; /*case 3*/
        if (__v0 == 2) goto __L5; /*case 2*/
        goto __L6; /* default */

        {
            __L1: /*case 1*/ 
            return "Inf";
            __L2: /*case 0*/ 
            return "NaN";
            __L3: /*case 4*/ 
            return "normal";
            __L4: /*case 3*/ 
            return "subnormal";
            __L5: /*case 2*/ 
            return "zero";
            __L6: /* default */ 
            return "unknown";
        }
        __L0:;
    }
}


extern int printf(char * __format, ...);

int main(void)
{
    printf("1.0/0.0 is %s\n", show_classification(.7976931348623157E+308));
    printf("0.0/0.0 is %s\n", show_classification(-nan(ind).0));
    printf("DBL_MIN/2 is %s\n", show_classification(1.1125369292536007e-308));
    printf("-0.0 is %s\n", show_classification(-0.0));
    printf("1.0 is %s\n", show_classification(1.0));
}


