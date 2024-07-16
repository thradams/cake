//en.cppreference.com/w/c/error/errno.html
#include <stdio.h>
#include <math.h>
#include <errno.h>
 
void show_errno(void)
{
    const char *err_info = "unknown error";
    switch (errno) {
    case EDOM:
        err_info = "domain error";
        break;
    case EILSEQ:
        err_info = "illegal sequence";
        break;
    case ERANGE:
        err_info = "pole or range error";
        break;
    case 0:
        err_info = "no error";
    }
    fputs(err_info, stdout);
    puts(" occurred");
}
 
int main(void)
{
    fputs("MATH_ERRNO is ", stdout);
    puts(math_errhandling & MATH_ERRNO ? "set" : "not set");
 
    errno = 0;
    1.0/0.0;
    show_errno();
 
    errno = 0;
    acos(+1.1);
    show_errno();
 
    errno = 0;
    log(0.0);
    show_errno();
 
    errno = 0;
    sin(0.0);
    show_errno();
}