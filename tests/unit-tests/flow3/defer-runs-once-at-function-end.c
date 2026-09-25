#pragma safety enable
#include <stdlib.h>

int get(void);

void defer_free_once(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    defer free(p); /* ok: not "p is moved" */
    if (get())
    {
    }
}

void defer_body_runs_once(void)
{
    int n = 0;
    defer
    {
        n++;
        compile_assert(n == 1);
    }
}
