#pragma safety enable
#include <stdlib.h>

/*
    The objects declared in the init-statement or in the condition of an if
    belong to the if: they are checked and their lifetime ends when it ends.
*/

int get(void);

void if_init_ends_with_the_if(void)
{
    int j = 0;
    int* q = &j;
    if (int i = get(); i)
    {
        q = &i;
    }
    *q = 1; //lint 31
}

void if_condition_declaration_ends_with_the_if(void)
{
    int j = 0;
    int* q = &j;
    if (int i = get())
    {
        q = &i;
    }
    else
    {
        q = &i;
    }
    *q = 1; //lint 31
}

void if_init_owner_released(void)
{
    if (int* _Owner p = malloc(sizeof(int)); p)
    {
        free(p);
    }
}
