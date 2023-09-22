#pragma once

/*   7.16     Variable arguments <stdarg.h>
1   The header <stdarg.h> declares a type and defines five macros, for advancing through a list of
    arguments whose number and types are not known to the called function when it is translated.
2   The macro
              __STDC_VERSION_STDARG_H__
              */

typedef char* va_list;

#define __STDC_VERSION_STDARG_H__ 202311L

 //type    va_arg(va_list ap, type);
#define va_arg(ap, type) (*((type*)0))
#define va_copy(dest, src) 
#define va_end(ap)
#define va_start(ap, ...)

 //void    va_copy(va_list dest, va_list src);
 //void    va_end(va_list ap);
 //void    va_start(va_list ap, ...);