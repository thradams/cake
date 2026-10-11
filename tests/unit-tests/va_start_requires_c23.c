/*
  The builtins are used directly: the va_start macro of older system
  <stdarg.h> headers requires two arguments.
*/

/* named last parameter: converted to __builtin_va_start(ap, n), no warning */
int a(int n, ...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap);
    __builtin_va_end(ap);
    return n;
}

/* no named parameter */
int b(...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap); //lint 126
    __builtin_va_end(ap);
    return 0;
}

/* unnamed last parameter */
int c(int, ...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap); //lint 126
    __builtin_va_end(ap);
    return 0;
}

int main(void)
{
    return a(0) + b() + c(0);
}
