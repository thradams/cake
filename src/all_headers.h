/*
    This file is useful to force cake to find all standard headers
    cake all_headers.h -H
*/


/* General utilities */
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

/* Input/output */
#include <stdio.h>

/* Strings and characters */
#include <ctype.h>
#include <string.h>

/* Math */
#include <math.h>
#include <float.h>
#include <complex.h>
#include <fenv.h>
//#include <tgmath.h>

/* Time and date */
#include <time.h>

/* Boolean and integer types */
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdnoreturn.h>
#include <uchar.h>

/* Threads and atomics (C11/C23) */
#include <threads.h>
//#include <stdatomic.h>

/* Optional I/O and extensions (C23) */
#include <errno.h>
#include <wchar.h>
#include <wctype.h>

/* C23 additions (if supported by your compiler) */
#if __STDC_VERSION__ >= 202311L
//#include <stdbit.h>
#include <stdbool.h> /* already included, but harmless */
//#include <stdckdint.h>
#include <uchar.h>   /* already included */
#include <assert.h>  /* already included */
#endif


//#include <Windows.h>