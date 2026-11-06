/* abort and exit from <stdlib.h> end the path, also when the system header declares them with __declspec(noreturn) */
#pragma safety enable

#include <stdlib.h>

void f1(int* _Nullable p)
{
    if (!p) abort();
    *p = 1;
}

void f2(int* _Nullable p)
{
    if (!p) exit(1);
    *p = 1;
}

void f3(int* _Nullable p)
{
    if (!p) _Exit(1);
    *p = 1;
}

void f4(int* _Nullable p)
{
    if (!p) quick_exit(1);
    *p = 1;
}
