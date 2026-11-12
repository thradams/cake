#pragma safety enable
#include <stdlib.h>

/*
    Moving an object inside a branch, when its value was set before the
    branch: the value is kept and marked moved, as when both happen in the
    same block. The pointer still has its target, it does not become
    "may be null".
*/

void use(int* p);

void moved_where_it_was_set(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == NULL)
        return;
    free(p);
    use(p); //lint 31
}

void moved_inside_a_branch(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == NULL)
        return;
    if (c)
    {
        free(p);
        use(p); //lint 31
    }
    else
    {
        free(p);
    }
}

void moved_inside_a_nested_branch(int c1, int c2)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == NULL)
        return;
    if (c1)
    {
        if (c2)
        {
            free(p);
            use(p); //lint 31
            return;
        }
    }
    free(p);
}
