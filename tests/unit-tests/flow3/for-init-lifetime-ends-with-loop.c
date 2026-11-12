#pragma safety enable
#include <stdlib.h>

/*
    The objects of the for-init belong to the for statement: their lifetime
    ends when the loop ends, with the state of every exit joined. break and
    continue do not end them.
*/

int get(void);

void for_init_ends_with_the_loop(void)
{
    int* q = NULL;
    for (int i = 0;;)
    {
        q = &i;
        break;
    }
    *q = 1; //lint 31
}

void released_before_break(void)
{
    for (int* _Owner p = malloc(sizeof(int));;)
    {
        free(p);
        break;
    }
}

void released_on_every_exit(void)
{
    for (int* _Owner p = malloc(sizeof(int)); p != NULL;)
    {
        free(p);
        p = NULL;
    }
}

void continue_keeps_for_init_alive(void)
{
    for (int i = 0; i < 3; i++)
    {
        int* q = &i;
        if (get())
            continue;
        *q = 2;
    }
}
