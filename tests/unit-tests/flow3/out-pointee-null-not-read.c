#pragma safety enable

/* an _Out pointee is only written by the callee: a null it holds before the call is not read (va_start's __va_start) */

void init(char * _Out * p);
void init_va(char * _Out * p, ...);

void null_before(void)
{
    char * s = { 0 };
    init(&s);
}

void null_before_variadic(void)
{
    char * s = { 0 };
    ((void)(init_va(&s, 1)));
}
