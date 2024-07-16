//en.cppreference.com/w/c/numeric/math/fpclassify.html
#include <stdio.h>
#include <math.h>
#include <float.h>
 
const char *show_classification(double x) {
    switch(fpclassify(x)) {
        case FP_INFINITE:  return "Inf";
        case FP_NAN:       return "NaN";
        case FP_NORMAL:    return "normal";
        case FP_SUBNORMAL: return "subnormal";
        case FP_ZERO:      return "zero";
        default:           return "unknown";
    }
}
int main(void)
{
    printf("1.0/0.0 is %s\n", show_classification(1/0.0));
    printf("0.0/0.0 is %s\n", show_classification(0.0/0.0));
    printf("DBL_MIN/2 is %s\n", show_classification(DBL_MIN/2));
    printf("-0.0 is %s\n", show_classification(-0.0));
    printf("1.0 is %s\n", show_classification(1.0));
}