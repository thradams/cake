
#define IS_SAME_TYPE(T1, T2) \
  _Generic(typeof(T1), typeof(T2): 1, default: 0)

int main()
{
  double const x = 78.9;
  double y = 78.9;
  double q = x;
  
   const double  * const p = &x;
  
   double  * const r = &y;
  
  char  * s = "test";
  
}
