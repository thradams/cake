/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double hypot(double __x, double __y);
extern double atan2(double __y, double __x);

int main(void)
{
    printf("(+1,+1) cartesian is (%f,%f) polar\n", hypot(1, 1), atan2(1, 1));
    printf("(+1,-1) cartesian is (%f,%f) polar\n", hypot(1, -1), atan2(1, -1));
    printf("(-1,-1) cartesian is (%f,%f) polar\n", hypot(-1, -1), atan2(-1, -1));
    printf("(-1,+1) cartesian is (%f,%f) polar\n", hypot(-1, 1), atan2(-1, 1));
    printf("atan2(0, 0) = %f atan2(0, -0)=%f\n", atan2(0, 0), atan2(0, -0.0));
    printf("atan2(7, 0) = %f atan2(7, -0)=%f\n", atan2(7, 0), atan2(7, -0.0));
}


