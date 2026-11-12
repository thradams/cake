#pragma safety enable
#include <stdlib.h>

/* a jump runs the defers of the scopes it leaves first: the state that reaches the join has their effects */

int get(void);

void defer_before_break(void)
{
    int n = 0;
    for (;;)
    {
        defer n = 1;
        break;
    }
    compile_assert(n == 1);
}

void defer_before_break_in_if(void)
{
    int n = 0;
    while (get())
    {
        defer n = 1;
        if (get())
            break;
    }
    compile_assert(n == 0 || n == 1);
}

void defer_before_continue(void)
{
    int n = 0;
    do
    {
        defer n = 1;
        continue;
    }
    while (0);
    compile_assert(n == 1);
}

void defer_before_throw(void)
{
    int n = 0;
    try
    {
        defer n = 1;
        throw;
    }
    catch
    {
        compile_assert(n == 1);
    }
}

void defer_before_goto(void)
{
    int n = 0;
    {
        defer n = 1;
        goto end;
    }
end:
    compile_assert(n == 1);
}

void defer_free_before_break(void)
{
    int* _Owner p = malloc(sizeof(int));
    for (;;)
    {
        defer free(p);
        break;
    }
    /* ok: p was released by the defer */
}

void defer_free_before_throw(void)
{
    int* _Owner p = malloc(sizeof(int));
    try
    {
        defer free(p);
        throw;
    }
    catch
    {
    }
    /* ok: p was released by the defer */
}
