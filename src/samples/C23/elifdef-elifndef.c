/*
  C23 preprocessing directives elifdef and elifndef N2645
  https://open-std.org/jtc1/sc22/wg14/www/docs/n2645.pdf
*/

#define Y

#ifdef X
#define VERSION 1
#elifdef  Y
#define VERSION 2
#else
#define VERSION 3
#endif

_Static_assert(VERSION == 2, "");
