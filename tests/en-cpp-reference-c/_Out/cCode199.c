//en.cppreference.com/w/c/numeric/fenv/feclearexcept.html
#include <fenv.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
 
/*
 * A possible implementation of hypot which makes use of many advanced
 * floating-point features.
 */
double hypot_demo(double a, double b) {
  const int range_problem = FE_OVERFLOW | FE_UNDERFLOW;
  feclearexcept(range_problem);
  // try a fast algorithm
  double result = sqrt(a * a + b * b);
  if (!fetestexcept(range_problem))  // no overflow or underflow
    return result;                   // return the fast result
  // do a more complicated calculation to avoid overflow or underflow
  int a_exponent,b_exponent;
  frexp(a, &a_exponent);
  frexp(b, &b_exponent);
 
  if (a_exponent - b_exponent > DBL_MAX_EXP)
    return fabs(a) + fabs(b);        // we can ignore the smaller value
  // scale so that fabs(a) is near 1
  double a_scaled = scalbn(a, -a_exponent);
  double b_scaled = scalbn(b, -a_exponent);
  // overflow and underflow is now impossible 
  result = sqrt(a_scaled * a_scaled + b_scaled * b_scaled);
  // undo scaling
  return scalbn(result, a_exponent);
}
 
int main(void)
{
  // Normal case takes the fast route
  printf("hypot(%f, %f) = %f\n", 3.0, 4.0, hypot_demo(3.0, 4.0));
  // Extreme case takes the slow but more accurate route
  printf("hypot(%e, %e) = %e\n", DBL_MAX / 2.0, 
                                DBL_MAX / 2.0, 
                                hypot_demo(DBL_MAX / 2.0, DBL_MAX / 2.0));
 
  return 0;
}