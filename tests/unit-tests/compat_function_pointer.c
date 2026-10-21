/* flow.md A15.3: a function pointer may point to a function that accepts more and promises more */
#pragma safety enable

void f_ptr(int * p);
void f_opt(int * _Opt p);
int * _Opt g_opt(void);
int * g_nonnull(void);

void f()
{
    void (*pf)(int * p) = f_opt;
    void (*pf2)(int * _Opt p) = f_ptr; //lint 54
    int * _Opt (*pg)(void) = g_nonnull;
    int * (*pg2)(void) = g_opt; //lint 54
}
